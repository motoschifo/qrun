# qrun - Minimalist Command Launcher for KDE Plasma

A lightweight, fast, and deterministic command launcher.
It's built with C++ and Qt6, offering a clean interface to launch commands, scripts, and open folders instantly.

Developed as a modern alternative to the classic "Run dialog", I use it on Arch Linux and KDE Plasma (Wayland).

## Features
* Lightning fast: no indexing, web searches
* Smart history: remembers your commands and provides a simple auto-complete popup
* Flexible filtering: multi-word search (e.g., typing "backup script" finds "daily_backup_script.sh").
* Smart navigation:
    * if input is an absolute path or a folder in ~/, it opens in file manager (Dolphin)
    * Otherwise, it executes the command via bash shell (interactive mode to respect your aliases and PATH)

## Environment
* OS: Arch Linux
* Desktop Environment: KDE Plasma (Wayland)
* Framework: Qt6

## Dependencies
To compile qrun, you need the following packages installed on your system:

```sudo pacman -S base-devel qt6-base cmake ```

## Installation & Compilation

1. Clone the repository:
   git clone https://github.com/motoschifo/qrun.git
   cd qrun

2. Build the project:
   mkdir build && cd build
   cmake ..
   make

3. Test the binary:
   ./qrun

## KDE Integration
To use qrun as your primary launcher (replacing Alt+F2):
1. Open System Settings -> Shortcuts -> Shortcuts.
2. Click Add New (or modify an existing one).
3. Set the command to the absolute path of your binary: /home/youruser/src/qrun/build/qrun
4. Assign your preferred shortcut (e.g., Meta+R or Alt+F2).

## TODO
- [ ] Configuration file: options and settings to a file (~/.config/qrun/qrun.conf)
- [ ] Theming: support for system-wide color schemes.

## License
GPL-3.0
