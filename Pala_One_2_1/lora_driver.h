// lora_driver.h — SX1262 LoRa hardware driver for Pala One
//
// This file only talks to the radio hardware. It knows nothing about
// Meshtastic, packet formats, encryption, or any higher-level protocol.
// Protocol logic lives entirely in loadable apps (e.g. examples/mesh_chat/).
//
// What lives here and why it cannot be in an app:
//   SPIClass / SX1262  — C++ objects; apps are position-independent C
//   loraRxIsr          — must be in IRAM; a loaded binary cannot be registered as an IRQ handler
//   receive queue      — ISR fires asynchronously; bytes must land somewhere before the app polls
//   loraInit / loraSleep /
//   loraRecv / loraSend — thin wrappers around RadioLib; exposed to apps via PalaAPI v4
#pragma once

#include <RadioLib.h>

// ── SX1262 pin assignments (Heltec Wireless Paper, WirelessPaper.h:41-47) ────
#define LORA_NSS   8
#define LORA_DIO1  14
#define LORA_RST   12
#define LORA_BUSY  13
#define LORA_SCK   9
#define LORA_MISO  11
#define LORA_MOSI  10

// ── Raw receive queue ─────────────────────────────────────────────────────────
#define LORA_RX_QUEUE   4
#define LORA_PKT_MAX    250

struct LoraRxEntry {
    uint8_t buf[LORA_PKT_MAX];
    int     len;
};
static LoraRxEntry g_loraRxQueue[LORA_RX_QUEUE];
static int g_loraRxHead = 0;
static int g_loraRxTail = 0;

// ── Hardware objects — must live in firmware (C++ / IRAM) ─────────────────────
static SPIClass      g_loraSPI(HSPI);
static SX1262        g_radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY, g_loraSPI);
static bool          g_loraReady = false;
static volatile bool g_loraRxFlag = false;

void IRAM_ATTR loraRxIsr() { g_loraRxFlag = true; }

// ── Node ID from chip MAC ─────────────────────────────────────────────────────
static uint32_t loraNodeId() {
    return (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF);
}

// ── PalaAPI v4: configure and start the radio ─────────────────────────────────
static int loraInit(float freq_mhz, float bw_khz, int sf, int cr,
                    uint8_t sync_word, int tx_power, int preamble, float tcxo_v) {
    g_loraSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    int state = g_radio.begin(freq_mhz, bw_khz, sf, cr, sync_word, tx_power, preamble, tcxo_v);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[LORA] Init failed: %d\n", state);
        return state;
    }
    g_radio.setDio1Action(loraRxIsr);
    state = g_radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("[LORA] startReceive failed: %d\n", state);
        return state;
    }
    g_loraReady = true;
    Serial.printf("[LORA] Ready — %.4f MHz SF%d BW%.0f node=!%08lx\n",
                  freq_mhz, sf, bw_khz, (unsigned long)loraNodeId());
    return 0;
}

// ── PalaAPI v4: put radio to sleep ────────────────────────────────────────────
static void loraSleep() {
    if (g_loraReady) {
        g_radio.sleep(false);
        g_loraReady = false;
    }
}

// ── Internal: drain ISR flag into receive queue ───────────────────────────────
static void loraProcessRx() {
    if (!g_loraReady || !g_loraRxFlag) return;
    g_loraRxFlag = false;
    size_t rxLen = g_radio.getPacketLength();
    if (rxLen >= 4 && rxLen <= LORA_PKT_MAX) {
        int nextHead = (g_loraRxHead + 1) % LORA_RX_QUEUE;
        if (nextHead != g_loraRxTail) {
            int state = g_radio.readData(g_loraRxQueue[g_loraRxHead].buf, rxLen);
            if (state == RADIOLIB_ERR_NONE) {
                g_loraRxQueue[g_loraRxHead].len = (int)rxLen;
                g_loraRxHead = nextHead;
            }
        }
    }
    g_radio.startReceive();
}

// ── PalaAPI v4: non-blocking raw receive ─────────────────────────────────────
static int loraRecv(uint8_t* buf, int maxlen) {
    loraProcessRx();
    if (g_loraRxHead == g_loraRxTail) return 0;
    LoraRxEntry& e = g_loraRxQueue[g_loraRxTail];
    int n = (e.len < maxlen) ? e.len : maxlen;
    memcpy(buf, e.buf, n);
    g_loraRxTail = (g_loraRxTail + 1) % LORA_RX_QUEUE;
    return n;
}

// ── PalaAPI v4: transmit raw bytes ────────────────────────────────────────────
static void loraSend(const uint8_t* buf, int len) {
    if (!g_loraReady || len <= 0) return;
    Serial.printf("[LORA] TX %d bytes\n", len);
    g_radio.transmit(buf, (size_t)len);
    g_radio.startReceive();
}
