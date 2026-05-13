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

The firmware includes an optional Apps menu. Each app can be independently included or excluded by commenting out its `#define` near the top of `Pala_One_2_1/Pala_One_2_1.ino`:

```cpp
// ── Apps: comment out any app you don't want compiled into the firmware ───────
#define APP_CLICK_COUNTER
```

If all apps are commented out, the Apps entry disappears from the library menu entirely.

### Adding a new app

1. Add a `#define APP_FOO` line in the apps block at the top of the `.ino`, and add `|| defined(APP_FOO)` to the `ANY_APP_DEFINED` chain directly below it.
2. Add a `"Foo",` entry inside an `#ifdef APP_FOO` slot in the `apps[]` array inside `drawAppsMenu()`.
3. Add a dispatch block inside the `doubleClick` branch of `handleModeApps()`:
   ```cpp
   #ifdef APP_FOO
   if (g_appsSelectedIndex == i) { mode = MODE_FOO; drawFoo(); return; }
   i++;
   #endif
   ```
4. Wrap the app's own mode value (`MODE_FOO`), state variables, draw function, and handler function each in `#ifdef APP_FOO`.

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
