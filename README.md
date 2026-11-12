<div align="center">
  <img src="data/pistachio-text.svg" alt="Pistachio Text icon" width="112" />

  # Pistachio Text

  A lightweight text and code editor for Linux, styled to match Pistachio Noir.
</div>

Pistachio Text is a single-document desktop editor built with Qt Quick (QML) and C++. It uses KDE's KSyntaxHighlighting for language-aware colours and provides matching Open and Save as dialogs.

## Features

- Syntax highlighting for languages supported by KSyntaxHighlighting
- Open, edit, and save one file at a time
- Find and Go to line controls
- Indentation-aware Enter and Tab behavior
- Preserves LF and CRLF line endings
- Rejects non-UTF-8 files and files larger than 10 MB
- Linux desktop entry and scalable application icon

## Requirements

- CMake 3.21 or newer
- Qt 6.9 or newer: Core, Gui, Qml, Quick, and Test
- KF6 Syntax Highlighting
- Ninja

## Build and run

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
./build/pistachio-text [file]
```

Install to `~/.local`:

```sh
cmake --install build --prefix ~/.local
```

The install includes the application, desktop launcher, and icon. To set it as the default plain-text editor:

```sh
xdg-mime default pistachio-text.desktop text/plain
```

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Ctrl+O` | Open a file |
| `Ctrl+S` | Save; prompts for a name on a new document |
| `Ctrl+Shift+S` | Save as |
| `Ctrl+F` | Find; `Enter` goes to the next match, `Shift+Enter` to the previous, `Esc` closes |
| `Ctrl+G` | Go to line |
| `Ctrl++` / `Ctrl+-` / `Ctrl+0` | Increase, decrease, or reset text size |
| `Ctrl+mouse wheel` | Zoom text |
| `Ctrl+W` / `Ctrl+Q` | Close; asks about unsaved changes |

In the Open and Save as dialogs, type to filter files. Enter a path beginning with `/` or `~` and press Enter to navigate. Use Up/Down to select, Enter to open, Esc to close, and Ctrl+H to show hidden files.

## Development checks

```sh
QT_QPA_PLATFORM=offscreen build/tst_backend
QT_QPA_PLATFORM=offscreen build/ptx-shots build/shots
cmake --build build --target all_qmllint
```

The screenshot check writes images to `build/shots/`. `TESTING.md` lists additional manual checks.

## Project layout

- `src/` — document handling, syntax highlighting, and file listing
- `qml/` — editor window, dialogs, controls, and theme
- `data/` — desktop entry and application icon
- `tests/` — Qt tests and screenshot checks
- `design/` — design boards

## License

Pistachio Text is licensed under the GNU General Public License v3.0 only. See [LICENSE](LICENSE).
