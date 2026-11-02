# Pistachio Text

A small text and code editor in the Pistachio Noir look, for Hyprland. One file at a time, line numbers, syntax colours for the languages KDE's KSyntaxHighlighting knows, and its own Open and Save as cards that match the shell. QML + C++ on Qt 6, like Pistachio Files and Notes.

## Build, run, install

Needs Qt 6.9+ (Core, Gui, Qml, Quick, Test), `syntax-highlighting` (KF6), CMake and Ninja.

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
build/pistachio-text [file]          # a file that doesn't exist yet is created by the first save
cmake --install build --prefix ~/.local   # app, desktop entry (absolute Exec) and hicolor icon
```

After installing, Pistachio Text shows in the launcher and in FM's Open with for text files. To make it the app that opens text (your call, reversible): `xdg-mime default pistachio-text.desktop text/plain`.

## Keys

| Key | Does |
| --- | --- |
| Ctrl+O | Open file |
| Ctrl+S | Save (Save as for a new file) |
| Ctrl+Shift+S | Save as |
| Ctrl+F | Find in file: Enter next, Shift+Enter previous, Esc closes |
| Ctrl+G | Go to line |
| Ctrl + / Ctrl - / Ctrl 0, Ctrl + wheel | Text bigger / smaller / 13 px, or zoom with the wheel |
| Ctrl+W, Ctrl+Q | Close (asks if there are unsaved changes) |
| Enter, Tab | Enter keeps the line's indent; Tab types the file's own indent (a tab, or 2 to 8 spaces) |
| In Open / Save as: type | filters the files; a `/` or `~` path + Enter goes there |
| In Open / Save as: Up, Down, Enter, Esc, Ctrl+H | pick a row, open it, clear the filter then close, show dotfiles |

Files that aren't UTF-8 text, or are over 10 MB, are refused with a line in the status bar. Line endings (LF or CRLF) are kept.

## Code layout

```
src/    Document (load, save, indent, line and column), Highlighter (KSyntaxHighlighting
        mapped to Theme colours), FolderLister (what the dialogs list), WheelZoom (Ctrl+wheel), main.cpp
qml/    Main (window), FileDialog (Open and Save as), ConfirmDialog, QuickBar (Find / Go to line),
        Theme and Icons (the only colours and glyphs), small widgets copied from pistachio-fm
tests/  tst_backend.cpp (QtTest), shots.cpp (offscreen screenshots + key checks)
data/   pistachio-text.desktop, pistachio-text.svg
design/ the boards (canvas-source/)
docs/   plan.md
```

If Ctrl+wheel doesn't zoom, run `PTX_DEBUG_WHEEL=1 build/pistachio-text` and scroll: it prints what the window receives.

Checks: `QT_QPA_PLATFORM=offscreen build/tst_backend`, `QT_QPA_PLATFORM=offscreen build/ptx-shots build/shots` (writes the pictures to `build/shots/`), `cmake --build build --target all_qmllint`.
