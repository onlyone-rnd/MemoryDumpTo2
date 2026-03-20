# MemoryDumpTo v2.0

![Platform](https://img.shields.io/badge/platform-Windows-0078D6)
![Debugger](https://img.shields.io/badge/debugger-x64dbg-6A5ACD)
![GUI](https://img.shields.io/badge/GUI-Qt-41CD52)
![License](https://img.shields.io/badge/license-MIT-green)

**MemoryDumpTo** is a x64dbg plugin with a QT interface that allows you to save and convert memory dumps
into source code-ready data declarations for several programming languages and assembly syntaxes.

The plugin can work with dump data taken directly from common x64dbg views such as **Disassembly**, **Memory Map**, and **Dump**.  
The conversion dialog is also **resizable**, allowing users to adjust the window size according to their workflow and screen space.

---

## Status

**MemoryDumpTo** is focused on practical, source-oriented memory export for x64dbg users who need fast results without unnecessary overhead.

---

## Features

- Save and convert memory dumps into source-ready text representations
- Directly process data selected from x64dbg views, including:
  - **Disassembly**
  - **Memory Map**
  - **Dump**
- Export memory content to multiple assembler syntaxes and programming languages
- User-resizable conversion dialog for a more flexible workflow
- Switch quickly between:
  - **BYTE**
  - **WORD**
  - **DWORD**
  - **QWORD**
- Copy generated output to the clipboard
- Save generated output to a file
- Fast workflow designed for reverse engineering and low-level analysis
- Full Qt-based GUI

---

## License

This repository is licensed under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

## Third-Party Components

This project uses **Qt** for the GUI layer.
Qt is licensed separately under its own licensing terms.
See [THIRD_PARTY_NOTICES](THIRD_PARTY_NOTICES.md)
 for third-party attribution and licensing information.

---

## GUI

**The entire GUI is implemented with Qt.**

This includes dialogs, controls, text editors, layout management, and UI interaction logic.  
Qt is used for the full graphical layer of the plugin.

---

## Supported Output Languages

### Assemblers
- MASM/UASM/JWasm/ASMC
- NASM/YASM
- FASM
- GAS/clang-as

### Native/Systems
- C/C++
- C#

### Scripting/Tooling
- Python

### Advanced
- Delphi/FreePascal/Lazarus
- Node.js

---

## Hotkeys

### General
- **Ctrl+C** — copy the generated source text to the clipboard
- **Ctrl+S** — save the generated source text to a file

### Data Width
- **Alt+B** — switch the output format to **BYTE**
- **Alt+W** — switch the output format to **WORD**
- **Alt+D** — switch the output format to **DWORD**
- **Alt+Q** — switch the output format to **QWORD**

These shortcuts are intended to keep the workflow fast and reduce friction while working inside x64dbg.

---

## Demo

![](https://github.com/onlyone-rnd/MemoryDumpTo2/blob/main/demo/demo.gif)

---

## Installation

### x64 Release
1. Download the **x64** release archive.
2. Extract the files.
3. Copy the plugin binaries to the appropriate **x64dbg x64 plugin directory**.
4. Start **x64dbg** and load the plugin.

### x86 Release
1. Download the **x86** release archive.
2. Extract the files.
3. Copy the plugin binaries to the appropriate **x64dbg x86 plugin directory**.
4. Start **x32dbg** and load the plugin.

---

## Release Packages

Recommended release assets:

- `MemoryDumpTo-x64.zip`
- `MemoryDumpTo-x86.zip`

---

## Build

### Requirements

- Visual Studio 2022
- Qt 5.12.12
- x64dbg plugin SDK
- Windows SDK
