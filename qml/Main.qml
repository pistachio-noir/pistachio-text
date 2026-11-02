pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls.Basic as Controls
import QtQuick.Layouts

// The editor's window, laid out as the Main board: file name and Open/Save on top, the text with its
// line numbers, a status line. The Open and Save as cards are the other two boards. The rounded
// corners, border and shadow around it are the compositor's.
Controls.ApplicationWindow {
    id: window

    required property string startFile
    property bool discardOnClose // set once the user has agreed to drop unsaved text
    readonly property bool dialogOpen: openCard.visible || saveCard.visible || confirmCard.visible
    readonly property alias document: doc // for ptx-shots
    readonly property alias editor: area
    readonly property alias highlighter: highlighter
    readonly property alias bar: quickBar
    readonly property alias confirmDialog: confirmCard
    readonly property alias openDialog: openCard
    readonly property alias saveDialog: saveCard
    readonly property int fontSize: settings.fontSize
    readonly property real rowHeight: area.contentHeight / Math.max(1, area.lineCount) // measured: the font's lineSpacing is under what the layout uses, and the gap grows with every line

    function join(dir: string, name: string): string {
        return (dir === "/" ? "" : dir) + "/" + name;
    }

    // where a dialog starts: the open file's folder, else the last one used, else home
    function startFolder(): string {
        const path = doc.path;
        return path !== "" ? path.slice(0, path.lastIndexOf("/")) || "/" : lister.folderFor(settings.lastFolder) || lister.folderFor("~");
    }

    function showOpen() {
        openCard.show(startFolder(), doc.name);
    }

    function showSave() {
        saveCard.show(startFolder(), doc.path !== "" ? doc.name : "Untitled.txt");
    }

    function showFind() {
        const picked = area.selectedText;
        quickBar.mode = "find";
        quickBar.open(picked !== "" && !picked.includes("\n") ? picked : quickBar.text);
    }

    function showGoto() {
        quickBar.mode = "goto";
        quickBar.open("");
    }

    function save() {
        if (doc.path === "")
            showSave();
        else
            doc.save(doc.path);
    }

    // Opening drops what's unsaved, so that asks first.
    function openFile(path: string) {
        if (!doc.modified) {
            load(path);
            return;
        }
        confirmCard.ask({
            title: "Open “" + path.slice(path.lastIndexOf("/") + 1) + "” anyway?",
            description: "“" + doc.name + "” has changes you haven't saved. Opening another file drops them.",
            action: "Discard changes",
            run: () => load(path)
        });
    }

    function load(path: string) {
        if (doc.load(path))
            settings.lastFolder = path.slice(0, path.lastIndexOf("/")) || "/";
        area.forceActiveFocus();
    }

    function saveTo(folder: string, name: string) {
        const path = join(folder, name);
        const write = () => {
            if (doc.save(path))
                settings.lastFolder = folder;
            area.forceActiveFocus();
        };
        if (path === doc.path || !lister.exists(path)) {
            write();
            return;
        }
        confirmCard.ask({
            title: "Replace “" + name + "”?",
            description: "A file with that name is already in this folder. Saving replaces it.",
            action: "Replace",
            run: write
        });
    }

    function zoom(by: int) {
        settings.fontSize = by === 0 ? 13 : Math.max(8, Math.min(32, settings.fontSize + by));
    }

    // Find: the next match from the selection (the previous one backwards), wrapping round.
    function find(backwards: bool, fromStart: bool) {
        const query = quickBar.text.toLowerCase();
        if (query === "")
            return;
        const text = area.text.toLowerCase();
        let at;
        if (backwards) {
            at = text.lastIndexOf(query, area.selectionStart - 1);
            if (at < 0)
                at = text.lastIndexOf(query);
        } else {
            at = text.indexOf(query, fromStart ? area.selectionStart : area.selectionEnd);
            if (at < 0)
                at = text.indexOf(query);
        }
        quickBar.invalid = at < 0;
        if (at >= 0)
            area.select(at, at + query.length);
    }

    function goToLine(line: int) {
        if (isNaN(line) || line < 1 || line > area.lineCount) {
            quickBar.invalid = true;
            return;
        }
        area.cursorPosition = area.positionAt(0, (line - 1) * rowHeight + rowHeight / 2);
        quickBar.dismiss();
    }

    width: 912
    height: 552
    minimumWidth: 640
    minimumHeight: 520
    visible: true
    title: doc.name + " — Text"
    color: Theme.bgAlt

    onClosing: (close) => {
        if (!doc.modified || discardOnClose)
            return;
        close.accepted = false;
        confirmCard.ask({
            title: "Close without saving?",
            description: "“" + doc.name + "” has changes you haven't saved.",
            action: "Discard changes",
            run: () => {
                discardOnClose = true;
                window.close();
            }
        });
    }

    Component.onCompleted: {
        if (startFile !== "")
            load(startFile);
        else
            area.forceActiveFocus();
    }

    Settings {
        id: settings
        category: "editor"
        property int fontSize: 13
        property string lastFolder
    }

    Document {
        id: doc
        textDocument: area.textDocument
    }

    Binding {
        target: doc
        property: "cursorPosition"
        value: area.cursorPosition
    }

    Highlighter {
        id: highlighter
        textDocument: area.textDocument
        path: doc.path
        keywordColor: Theme.lavender
        functionColor: Theme.sageBlue
        typeColor: Theme.gold
        stringColor: Theme.teal
        numberColor: Theme.clay
        commentColor: Theme.subtext
    }

    FolderLister {
        id: lister
    }

    WheelZoom {
        enabled: !window.dialogOpen
        onZoom: (steps) => window.zoom(steps)
    }

    FontMetrics {
        id: metrics
        font: area.font
    }

    Shortcut { sequences: [StandardKey.Open]; enabled: !window.dialogOpen; onActivated: window.showOpen() }
    Shortcut { sequences: [StandardKey.Save]; enabled: !window.dialogOpen; onActivated: window.save() }
    Shortcut { sequence: "Ctrl+Shift+S"; enabled: !window.dialogOpen; onActivated: window.showSave() }
    Shortcut { sequences: [StandardKey.Find]; enabled: !window.dialogOpen; onActivated: window.showFind() }
    Shortcut { sequence: "Ctrl+G"; enabled: !window.dialogOpen; onActivated: window.showGoto() }
    Shortcut { sequences: [StandardKey.ZoomIn, "Ctrl+="]; enabled: !window.dialogOpen; onActivated: window.zoom(1) }
    Shortcut { sequences: [StandardKey.ZoomOut]; enabled: !window.dialogOpen; onActivated: window.zoom(-1) }
    Shortcut { sequence: "Ctrl+0"; enabled: !window.dialogOpen; onActivated: window.zoom(0) }
    Shortcut { sequences: ["Ctrl+W", "Ctrl+Q"]; enabled: !window.dialogOpen; onActivated: window.close() }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Rectangle {
                implicitWidth: nameRow.implicitWidth + 36
                implicitHeight: 44
                radius: Theme.tileRadius
                color: Theme.overlay

                Row {
                    id: nameRow
                    anchors.centerIn: parent
                    spacing: 10

                    Icon {
                        anchors.verticalCenter: parent.verticalCenter
                        size: 18
                        paths: Icons.file
                        color: Theme.sageBlue
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: doc.name
                        textFormat: Text.PlainText
                        color: Theme.textBright
                        font.family: Theme.fontUi
                        font.pixelSize: 13
                        font.weight: Font.Bold
                    }

                    Rectangle {
                        visible: doc.modified
                        anchors.verticalCenter: parent.verticalCenter
                        width: 6
                        height: 6
                        radius: 3
                        color: Theme.subtext
                        Accessible.name: "Unsaved changes"
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            PillButton {
                icon: Icons.folderOpen
                text: "Open"
                hint: "Ctrl O"
                onClicked: window.showOpen()
            }

            PillButton {
                icon: Icons.save
                text: "Save"
                hint: "Ctrl S"
                onClicked: window.save()
            }
        }

        QuickBar {
            id: quickBar
            Layout.fillWidth: true
            onEdited: if (mode === "find") window.find(false, true)
            onSubmitted: (backwards) => mode === "find" ? window.find(backwards, false) : window.goToLine(parseInt(text))
            onDismissed: area.forceActiveFocus()
        }

        Rectangle {
            id: box

            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.tileRadius
            color: Theme.bg
            clip: true

            readonly property int gutter: 44
            readonly property int pad: 14

            // the line the cursor is on, behind its number and its text
            Rectangle {
                x: 10
                y: box.pad + (doc.line - 1) * window.rowHeight - flick.contentY
                width: box.width - 20
                height: window.rowHeight
                radius: 8
                color: Theme.overlay
            }

            // line numbers: only the rows on screen exist, so a long file stays cheap
            Item {
                x: 10
                y: box.pad
                width: box.gutter
                height: flick.height
                clip: true

                Repeater {
                    model: Math.ceil(flick.height / window.rowHeight) + 1

                    Text {
                        id: number

                        required property int index
                        readonly property int line: Math.floor(flick.contentY / window.rowHeight) + index + 1

                        visible: line <= area.lineCount
                        y: (line - 1) * window.rowHeight - flick.contentY
                        width: box.gutter - 14
                        height: window.rowHeight
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        text: line
                        color: line === doc.line ? Theme.textBright : Theme.subtext
                        font.family: Theme.fontMono
                        font.pixelSize: window.fontSize
                        font.features: { "tnum": 1 }
                    }
                }
            }

            Flickable {
                id: flick
                x: 10 + box.gutter
                y: box.pad
                width: box.width - x - 10
                height: box.height - 2 * box.pad
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                Controls.ScrollBar.vertical: ThinScrollBar {}

                Controls.TextArea.flickable: Controls.TextArea {
                    id: area
                    objectName: "editor"

                    leftPadding: 6
                    rightPadding: 10
                    topPadding: 0
                    bottomPadding: 0
                    wrapMode: TextEdit.NoWrap
                    textFormat: TextEdit.PlainText
                    persistentSelection: true // the match Find picked stays marked while its bar has focus
                    selectByMouse: true
                    font.family: Theme.fontMono
                    font.pixelSize: window.fontSize
                    color: Theme.text
                    selectionColor: Theme.overlay2
                    selectedTextColor: Theme.textBright
                    tabStopDistance: metrics.advanceWidth("    ")
                    background: null
                    Accessible.name: "Text"

                    cursorDelegate: Rectangle {
                        id: caret

                        width: 2
                        radius: 1
                        color: Theme.pistachio
                        visible: area.activeFocus

                        Timer {
                            id: blink
                            interval: 530
                            repeat: true
                            running: caret.visible
                            onTriggered: caret.opacity = caret.opacity > 0 ? 0 : 1
                        }

                        Connections {
                            target: area
                            function onCursorPositionChanged() {
                                caret.opacity = 1;
                                blink.restart();
                            }
                        }
                    }

                    // Enter keeps the line's indent; Tab types the file's own indent, in place of a selection.
                    Keys.onPressed: (event) => {
                        const enter = event.key === Qt.Key_Return || event.key === Qt.Key_Enter;
                        if (event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.MetaModifier)
                                || !(enter || (event.key === Qt.Key_Tab && !(event.modifiers & Qt.ShiftModifier))))
                            return;
                        event.accepted = true;
                        if (area.selectedText !== "")
                            area.remove(area.selectionStart, area.selectionEnd);
                        const at = area.cursorPosition;
                        const text = enter ? "\n" + doc.indentAt(at) : doc.indentUnit;
                        area.insert(at, text);
                        area.cursorPosition = at + text.length;
                    }
                }
            }

        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 6
            Layout.rightMargin: 6

            Text {
                text: "Ln " + doc.line + ", Col " + doc.column
                color: Theme.subtext
                font.family: Theme.fontUi
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.features: { "tnum": 1 }
            }

            // not on the board: why a file wouldn't open or save
            Text {
                Layout.fillWidth: true
                Layout.leftMargin: 14
                Layout.rightMargin: 14
                text: doc.error
                textFormat: Text.PlainText
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                color: Theme.terracotta
                font.family: Theme.fontUi
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }

            Text {
                text: highlighter.language + " · " + doc.indentLabel + " · UTF-8 · " + doc.eolLabel
                color: Theme.subtext
                font.family: Theme.fontUi
                font.pixelSize: 11
                font.weight: Font.DemiBold
            }
        }
    }

    FileDialog {
        id: openCard
        onPicked: (folder, name) => {
            close();
            window.openFile(window.join(folder, name));
        }
        onClosed: area.forceActiveFocus()
    }

    FileDialog {
        id: saveCard
        saving: true
        onPicked: (folder, name) => {
            close();
            window.saveTo(folder, name);
        }
        onClosed: area.forceActiveFocus()
    }

    ConfirmDialog {
        id: confirmCard
        onClosed: area.forceActiveFocus()
    }
}
