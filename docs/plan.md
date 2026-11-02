# pistachio-text: plan (approved 2026-11-01)

Status: **built 2026-11-02 (all four milestones). Where the build differs from this plan, `CLAUDE.md` "Decisions" says so: one `FileDialog`, one `QuickBar`, no board line height.**

## Goal and v1 scope

A single-document text and code editor in Pistachio Noir: line numbers, syntax highlighting, custom Open and Save-as dialogs, exactly as the boards in `design/canvas-source/` (`Main`, `Open`, `Save`).

In v1 beyond the boards (owner's picks): Find in file (Ctrl+F), Go to line (Ctrl+G), text zoom (Ctrl +/-/0).

Out: tabs or splits, Settings dialog, word wrap, replace, autosave, watching the file for outside changes, encoding or line-ending switching, auto-closing brackets, drag and drop, IPC.

## Architecture

Qt 6.12, C++ backend, one static QML module `Pistachio.Text` (copy the CMake pattern from `pistachio-fm`). The only data is the user's own file; `QSettings` keeps the zoom size and last folder.

- `Document`: load and save through `QSaveFile`. Keeps LF or CRLF as found. Refuses invalid UTF-8, NUL bytes and files over 10 MB with a one-line message. Detects tabs or spaces (status bar and the Tab key). Knows path, modified flag and language name.
- `Highlighter`: KSyntaxHighlighting (6.31 installed; Notes uses it). Language comes from the file name. Token styles map to Theme colours: keyword lavender, function sageBlue, type gold, string teal, comment italic subtext. Highlighting is capped per line (known limit: minified one-line files).
- `FolderLister`: one folder for the dialogs, folders first, dotfiles hidden, Ctrl+H toggles.
- QML: `Main`, `OpenDialog`, `SaveDialog` (share one file list), `ConfirmDialog`, `FindBar`, `GotoPopup`; `Theme`, `Icons`, `Icon`, `Shadow` copied from `pistachio-fm/qml` with the canonical property names.
- Window: plain surface 912x552 (the board's card without its wallpaper margin). Hyprland draws corners, border and shadow.

Behaviour fixed by the boards: one document at a time, manual save, grey dot beside the file name while unsaved, the current line shown as an overlay chip with a pistachio caret, status bar `Ln, Col` on the left and `Language · Spaces: 4 · UTF-8 · LF` on the right (display only).

Behaviour decided here: Ctrl+S on Untitled opens Save as; Ctrl+Shift+S always does. The Open dialog's path pill shows the folder; typing filters names, and a typed path starting with `/` or `~` plus Enter jumps there. Enter keeps the line's indentation; Tab inserts the file's own indent unit (a tab, or 4 spaces).

Keys: Ctrl+O, Ctrl+S, Ctrl+Shift+S, Ctrl+W and Ctrl+Q (confirm if unsaved), Ctrl+F, Ctrl+G, Ctrl +/-/0, Esc closes find or go-to-line.

## Departures from the boards (owner approved the idea, looks need sign-off by screenshot)

1. `ConfirmDialog` (unsaved changes on Open, close; overwrite on Save as). Starts on Cancel. Copied from Notes.
2. `FindBar`.
3. `GotoPopup`.
4. A one-line terracotta error in the status bar for failed loads and saves.

## Suite integration

`data/pistachio-text.desktop`: absolute `Exec` and `TryExec` rewritten at install (copy FM's `install(CODE ...)`), `MimeType` for text files so FM's Open with lists it, `StartupWMClass=pistachio-text`, `%F`, hicolor SVG icon. A file path on the command line opens that file. Wayland app id `pistachio-text`. Making it the default editor is the owner's own `xdg-mime` call.

## Layout

```
pistachio-text/
  CLAUDE.md README.md TESTING.md CMakeLists.txt .gitignore
  design/canvas-source/{canvas.json,Main,Open,Save}.dc.html
  docs/plan.md
  src/    main.cpp Document.* Highlighter.* FolderLister.*
  qml/    Main OpenDialog SaveDialog FileList ConfirmDialog FindBar GotoPopup
          Theme Icons Icon Shadow (+ small widgets copied from FM/Notes)
  tests/  tst_backend.cpp shots.cpp
  data/   pistachio-text.desktop  pistachio-text.svg
```

## Testing

- `tests/tst_backend.cpp` (QtTest, offscreen, temp files): save round-trip, CRLF kept, binary and oversize refused, indent detection, language by file name, folder listing.
- `tests/shots.cpp`: offscreen screenshots of the Main, Open and Save boards plus the four departures, with PASS/FAIL checks. Never touches real drives.
- `all_qmllint` stays quiet. Run with `QT_FORCE_STDERR_LOGGING=1`.
- `TESTING.md` for the owner: real open from FM, Ctrl+S and Save as on real folders, a 5 MB file, the look next to the shell.

## Milestones

1. Window with Theme and the Main board layout, sample code. Screenshot next to the board.
2. Real open and save: `Document`, both dialogs, dirty dot, key bindings, status bar position, command-line file.
3. Highlighting, `ConfirmDialog`, find, go to line, zoom. Screenshots of the departures for sign-off.
4. Desktop entry, icon, install step, README, TESTING, suite database entries.

## Risks and database notes

G-11 (prefix Controls import), G-12/G-13/G-63 (popups and dialogs: take focus in `onOpened`), G-57 (undo must restore the caret), G-59 (`lastIndexOf` at 0), G-60 (full `QQuickTextDocument` include), G-65 (no icon theme offscreen), G-67 (tests call `rehighlight()`), G-14 (`QT_FORCE_STDERR_LOGGING=1`), G-19 (absolute `Exec`, hicolor icon).
