# pala-one-firmware
Pala One — A tiny E-Ink reader project by Paul Lagier

The goal of the project was to create a simple, distraction-free reading device that feels minimal, portable and easy to build while still looking and behaving more like a real product than a typical DIY electronics project.

## Contributing

If you improve the firmware, add features or fix bugs, feel free to open a pull request.
Please clearly mention:
- which board version(s) you tested on (V1.1, V1.2, or both)
- what was changed
- how it was tested

## Board Versions

There are currently two supported Heltec Wireless Paper versions:
- `Heltec V1.1`
- `Heltec V1.2`

The board version is usually printed on the back of the PCB.

Both versions are built from the same source file (`Pala_One_2_1/Pala_One_2_1.ino`). Open it in Arduino IDE and uncomment the `#define` at the top that matches your hardware before compiling.

## Apps

Apps are compiled separately and uploaded to the device via the WiFi web interface — no firmware rebuild needed. The Apps menu discovers all `.bin` files in `/apps/` on the device and lists them by name.

### Building an app

You need:
- The `xtensa-esp32s3-elf-gcc` cross-compiler, which ships with the Arduino ESP32 board package. On Linux it is found under `~/.arduino15/packages/esp32/tools/esp-x32/<version>/bin/`; the `Makefile` locates it automatically.
- `python3` (for the post-build step that patches the entry point offset into the binary).

An app is a single C file that includes `pala_app.h` and `pala_api.h` from the firmware source and exports a `void app_main(const PalaAPI* api)` entry point. The header struct at the start of the binary carries the display name and API version check.

See `examples/click_counter/` for a complete working example with a `Makefile`.

```bash
cd examples/click_counter
make
# produces click_counter.bin
```

### Uploading an app

1. Select **Upload** from the library menu on the device.
2. Connect to the `PALA-XXXXXX` WiFi network (password: `palaread`).
3. Open `http://192.168.4.1` in a browser.
4. Use the **Upload app (.bin)** card to upload your `.bin` file.
5. Triple-click to exit upload mode — the app will appear in the Apps menu immediately.

### App API

Apps communicate with the firmware through the `PalaAPI` function pointer table passed to `app_main`. Available functions (v1):

| Function | Description |
|---|---|
| `clearScreen()` | Clear the display buffer and prepare a new frame |
| `drawHeader(title)` | Draw the standard section header bar |
| `drawTextAt(x, y, text, bold)` | Draw text at a pixel position |
| `drawCenteredLarge(text)` | Draw text centred on screen in a large font |
| `refreshDisplay()` | Push the frame buffer to the e-ink panel |
| `waitForEvent()` | Block until a button gesture; returns `PALA_CLICK` / `PALA_DOUBLE` / `PALA_TRIPLE` / `PALA_LONG` |
| `snprintf_wrap(buf, len, fmt, ...)` | Standard `snprintf` |

Return from `app_main` to exit back to the Apps menu. Apps decide their own exit gesture — the firmware does not impose one.

**Constraints (v1):**
- Apps must be compiled `-fPIC -mlongcalls` (position-independent).
- Apps must not use static mutable variables — the loader does not patch `.data` relocations.
- Maximum binary size: 48 KB.

## Features

- TXT book support
- Adjustable font size
- Adjustable line spacing
- Deep sleep mode
- Reading progress saving
- USB-C charging
- Lightweight portable design
- Open-source firmware

## Hardware

Pala One is based on:
- Heltec Wireless Paper
- 3D printed housing
- LiPo battery

## Downloads

This repository contains the firmware source code for the project.

Additional files such as:
- STL files
- STEP files
- assembly guides
- printable files
- project downloads

are available separately via Ko-fi:

https://ko-fi.com/s/e14ed892ea

## Community & Modifications

Community improvements, forks and firmware modifications are welcome.  
If you build your own version or improve the project, feel free to share it with the community.

## License & Copyright

The firmware in this repository is provided for personal and educational use.

Please do not:
- reupload paid project files
- redistribute complete download packages
- resell the project files
- commercially redistribute modified versions of paid assets

The design, branding, documentation and paid project assets remain copyright © Paul Lagier.

---

Created by Paul Lagier
