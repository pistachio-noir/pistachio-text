@~/Desktop/pistachio-suite/CLAUDE.md

# pistachio-text

Single-document text and code editor for the Pistachio suite. QML + C++/Qt 6, as `pistachio-fm` and `pistachio-notes`. Plan: `docs/plan.md`. Boards: `design/canvas-source/` (`Main`, `Open`, `Save`).

**Status (2026-11-02): v1 built (milestones 1-4); 11 backend tests, 38 screenshot checks and qmllint pass. Not hand-tested by the owner yet (`TESTING.md`).** Build: `cmake -S . -B build -G Ninja && cmake --build build`; run `build/pistachio-text [file]`; checks `QT_QPA_PLATFORM=offscreen build/tst_backend` and `build/ptx-shots build/shots`.

## Decisions (already made)

- Standalone app, app id and Wayland class `pistachio-text`, own repo under `github.com/sxnvte666/` (the owner creates it; don't `git init`, commit or push).
- One document at a time, manual save (no autosave), no tabs.
- v1 extras beyond the boards: Find in file, Go to line, text zoom. Everything else in `docs/plan.md` "Out" stays out.
- Highlighting is KSyntaxHighlighting, mapped onto Theme colours (lavender keyword, sageBlue function, gold type, teal string, italic subtext comment).
- Departures from the boards (look needs the owner's sign-off by screenshot): `ConfirmDialog` (starts on Cancel; unsaved changes on Open and close, overwrite on Save as), `QuickBar` (one row under the toolbar that serves both Find, Ctrl+F, and Go to line, Ctrl+G; it replaced the planned separate FindBar and GotoPopup), the terracotta error line in the status bar. List any further departure here.
- Lines are the font's own height (about 18 px at 13 px text), not the board's 22 px: `TextEdit` has no line height, and setting one per block would put a step on the undo stack with every zoom.
- Open and Save as are one component, `FileDialog.qml` (`saving: true` for Save as); the two boards differ only in the file-name field, the folders-only list and the button.
- Find has no match highlighting beyond the selection of the current match (Enter next, Shift+Enter previous, wraps).
- Ctrl+wheel zoom is `src/WheelZoom` (an app-wide event filter), not a QML WheelHandler: WheelHandlers on the editor passed the simulated tests but never zoomed with the owner's real mouse (2026-11-02). `PTX_DEBUG_WHEEL=1` logs each wheel event the window gets.
- Settings live in `QSettings` (organisation `pistachio`, group `editor`): `fontSize`, `lastFolder`.
- Saves use `QSaveFile`; line endings are kept; invalid UTF-8, NUL bytes and files over 10 MB are refused.
- Registers a launcher entry with `MimeType` so FM's Open with lists it; becoming the default editor is the owner's `xdg-mime` call.

## Style rules

Colours only from `qml/Theme.qml` (copied from `pistachio-fm`, canonical names). Pistachio marks active things only. Manrope for UI, JetBrains Mono for code and paths. Radius 28 panels, 20 tiles, round icon buttons. Import Quick Controls as `QtQuick.Controls.Basic as Controls`.

## Working rules

Boards first; no UI that isn't on a board or listed under Departures. Docs in step with code. Verify by running (`QT_FORCE_STDERR_LOGGING=1`) and looking at the screenshots before claiming anything. No commits or pushes unprompted.

## Gotchas that apply

G-11, G-12, G-13, G-63 (popup focus), G-14 (logging), G-19 (absolute Exec, hicolor icon), G-57 (undo caret), G-59 (`lastIndexOf` at 0), G-60 (`QQuickTextDocument` include), G-65, G-67 (`rehighlight()` in tests). Details: `python3 ~/Desktop/pistachio-suite/db/q.py gotchas --scope qt-app`.

## Known limits (marked `shortcut:` in the code)

- `toPlainText()` turns a no-break space (U+00A0) into a plain space on save.
- Indent detection looks at the first 2000 lines; odd continuation lines can give a wrong width.
- A line over 4000 characters stays uncoloured.
- No horizontal scroll bar (trackpad or Shift+wheel scroll sideways; the caret scrolls into view).
- The folder lists in the dialogs aren't capped; a folder of tens of thousands of files is slow to open.

## Open questions

None. `pistachio-text` isn't in the suite database yet (`pistachio-suite-update`).
