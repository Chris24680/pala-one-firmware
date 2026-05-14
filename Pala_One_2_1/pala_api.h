#pragma once
#include <stdint.h>
#include <stdarg.h>

// Firmware-to-app API v1 — field order is frozen once shipped.
// To extend, append new fields and bump PALA_API_VERSION in pala_app.h.
typedef struct {
    void     (*clearScreen)(void);
    void     (*drawHeader)(const char* title);
    void     (*drawTextAt)(int x, int y, const char* text, int bold);
    void     (*drawCenteredLarge)(const char* text);
    void     (*refreshDisplay)(void);
    uint8_t  (*waitForEvent)(void);       // blocks until a gesture; returns PALA_CLICK/DOUBLE/TRIPLE/LONG
    int      (*snprintf_wrap)(char* buf, int len, const char* fmt, ...);
    uint8_t  (*pollEvent)(void);          // non-blocking; returns 0 if no event ready
    uint32_t (*millisNow)(void);          // current time in milliseconds
    int      (*buttonPressed)(void);      // 1 if button currently held, 0 otherwise
    void     (*delayMs)(uint32_t ms);     // yield for ms milliseconds
    uint32_t (*pendingPresses)(void);     // count of individual short press-release events since last call; bypasses multi-click grouping
} PalaAPI;
