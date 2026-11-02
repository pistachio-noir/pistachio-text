# Hand tests

What the automated checks (`tst_backend`, `ptx-shots`) can't see. Tick with the legend in `~/Desktop/pistachio-suite/docs/working-with-the-owner.md`.

## Looks (next to the boards in `design/canvas-source/`)

- [ ] Main: toolbar pills, the grey dot after a change, the current-line chip, the pistachio caret, status line
- [ ] Open file card: pistachio row, sizes, five rows, Cancel / Open
- [ ] Save as card: file name field with the caret, folders only, Cancel / Save
- [ ] Fits in with the shell and FM (colours, radii, fonts)
- [ ] The extras that aren't on a board: the Replace / Discard dialog, the Find and Go to line row, the red error line, 18 px lines instead of 22

## Files

- [ ] Open a real file from Ctrl+O; the text, the language name and "Spaces: N / Tabs" are right
- [ ] Save, then check the file on disk (`cat`, or open it in another editor); a CRLF file stays CRLF
- [ ] Save as to a new name, and over an existing file (asks first)
- [ ] `pistachio-text path/to/file` from a terminal, and a path that doesn't exist yet
- [ ] FM: Open with lists Pistachio Text on a `.txt`, `.rs`, `.json` and opens it
- [ ] A big file (a few MB of log): opens, scrolls and types without lag
- [ ] A binary file and an over-10 MB file are refused with a message, and the open text stays
- [ ] A read-only file or folder: Save shows an error and keeps the text

## Editing

- [ ] Enter keeps the indent, Tab types the right indent, Ctrl+Z / Ctrl+Shift+Z undo and redo (the caret lands where the change was)
- [ ] Ctrl+F finds in any case and wraps round; no match turns the text red
- [ ] Ctrl+G jumps to a line; a number past the end turns red
- [ ] Ctrl + / Ctrl - and Ctrl + mouse wheel / trackpad scroll zoom (the wheel alone still scrolls), and the size is still there after restarting
- [ ] Close with unsaved changes asks; Esc / Cancel keeps the window

## Highlighting

- [ ] Rust, C++, Python, JSON, Markdown, shell, a plain `.txt`: keywords lavender, functions sageBlue, types gold, strings teal, numbers clay, comments italic
