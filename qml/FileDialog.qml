pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic as Controls

// The Open file and Save as boards: one card over the dimmed window with the folder's entries. Open
// lists files too and its path pill filters them as you type (a typed / or ~ path + Enter goes
// there); Save as lists folders only and asks for a file name. Enter takes the picked row (a folder
// opens, a file is chosen); Ctrl+H shows dotfiles. picked(folder, name) is the answer.
Controls.Popup {
    id: root

    property bool saving
    property string folder
    property string current // Open: the file already open, picked first
    property bool hidden
    property var entries: []
    property string filter

    readonly property var rows: {
        const q = filter.toLowerCase();
        const shown = entries.filter(e => q === "" || e.name.toLowerCase().includes(q));
        if (lister.parentOf(folder) === folder)
            return shown;
        return [{ name: "Parent folder", path: lister.parentOf(folder), folder: true, size: "", parent: true }].concat(shown);
    }
    readonly property int visibleRows: saving ? 3 : 5
    readonly property string fileName: nameField.text.trim()
    readonly property bool nameOk: fileName !== "" && fileName !== "." && fileName !== ".." && !fileName.includes("/")

    signal picked(string folder, string name)

    // Open: the folder to start in, and the file open now. Save as: the folder and the suggested name.
    function show(startFolder: string, name: string) {
        hidden = false;
        current = saving ? "" : name;
        nameField.text = saving ? name : "";
        go(startFolder);
        open();
    }

    function go(path: string) {
        folder = path;
        filter = "";
        pill.text = "";
        entries = lister.list(path, hidden, saving);
        list.currentIndex = saving ? -1 : firstPick();
        list.positionViewAtIndex(Math.max(list.currentIndex, 0), ListView.Contain);
    }

    // Open: the file open now, else the first file, else the first row
    function firstPick(): int {
        const at = rows.findIndex(r => !r.folder && r.name === current);
        if (at >= 0)
            return at;
        const file = rows.findIndex(r => !r.folder);
        return file >= 0 ? file : rows.length > 0 ? 0 : -1;
    }

    function move(by: int) {
        list.currentIndex = Math.max(0, Math.min(rows.length - 1, list.currentIndex + by));
    }

    // Enter, double click: a folder row opens; otherwise Open chooses the file and Save as saves.
    function accept() {
        const row = rows[list.currentIndex];
        if (row && row.folder)
            go(row.path);
        else if (saving && nameOk)
            picked(folder, fileName);
        else if (!saving && row)
            picked(folder, row.name);
    }

    parent: Controls.Overlay.overlay
    anchors.centerIn: parent
    width: 440
    padding: 28
    modal: true
    focus: true
    closePolicy: Controls.Popup.CloseOnEscape | Controls.Popup.CloseOnPressOutside

    onOpened: (saving ? nameField : pill).forceActiveFocus()
    onFilterChanged: list.currentIndex = saving ? -1 : firstPick()

    Controls.Overlay.modal: Rectangle {
        color: Theme.scrim
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: Theme.motionDuration; easing.type: Theme.easing }
        NumberAnimation { property: "scale"; from: 0.97; to: 1; duration: Theme.motionDuration; easing.type: Theme.easing }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; to: 0; duration: Theme.hoverDuration; easing.type: Theme.easing }
    }

    FolderLister {
        id: lister
    }

    background: Item {
        Shadow {
            anchors.fill: card
            radius: card.radius
            kind: Theme.shadowModal
        }

        Rectangle {
            id: card
            anchors.fill: parent
            radius: Theme.panelRadius
            color: Theme.bgAlt
            border.color: Theme.overlay2
            border.width: 1
        }
    }

    contentItem: Column {
        spacing: 14

        Shortcut {
            sequence: "Ctrl+H"
            enabled: root.opened
            onActivated: {
                root.hidden = !root.hidden;
                root.go(root.folder);
            }
        }

        Row {
            spacing: 14

            Rectangle {
                width: 44
                height: 44
                radius: 22
                color: Theme.overlay2

                Icon {
                    anchors.centerIn: parent
                    size: 18
                    paths: root.saving ? Icons.save : Icons.folderOpen
                    color: Theme.sageBlue
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                Text {
                    text: root.saving ? "Save as" : "Open file"
                    color: Theme.textBright
                    font.family: Theme.fontUi
                    font.pixelSize: 18
                    font.weight: Font.ExtraBold
                }

                Text {
                    text: root.saving ? "Name the file and choose a folder" : "Pick a text or code file to edit"
                    color: Theme.subtext
                    font.family: Theme.fontUi
                    font.pixelSize: 12
                    font.weight: Font.Medium
                }
            }
        }

        Column {
            visible: root.saving
            width: parent.width
            spacing: 8

            Text {
                text: "File name"
                color: Theme.subtext
                font.family: Theme.fontUi
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.66
                font.capitalization: Font.AllUppercase
            }

            Rectangle {
                width: parent.width
                height: 44
                radius: 22
                color: Theme.bg

                TextInput {
                    id: nameField
                    anchors {
                        fill: parent
                        leftMargin: 18
                        rightMargin: 18
                    }
                    verticalAlignment: TextInput.AlignVCenter
                    clip: true
                    selectByMouse: true
                    color: Theme.textBright
                    selectionColor: Theme.overlay2
                    selectedTextColor: Theme.textBright
                    font.family: Theme.fontUi
                    font.pixelSize: 13
                    font.weight: Font.Bold
                    cursorDelegate: Rectangle {
                        width: 2
                        color: Theme.pistachio
                    }
                    Accessible.name: "File name"

                    onAccepted: root.accept()
                    Keys.onUpPressed: root.move(-1)
                    Keys.onDownPressed: root.move(1)
                }
            }
        }

        Column {
            width: parent.width
            spacing: 8

            Text {
                visible: root.saving
                text: "Folder"
                color: Theme.subtext
                font.family: Theme.fontUi
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.66
                font.capitalization: Font.AllUppercase
            }

            Rectangle {
                width: parent.width
                height: 44
                radius: 22
                color: Theme.bg

                Icon {
                    id: pillIcon
                    x: 16
                    anchors.verticalCenter: parent.verticalCenter
                    size: 18
                    paths: root.saving ? Icons.folder : Icons.search
                    color: Theme.subtext
                }

                // Open: the folder shown until you type; Save as: just the folder
                TextInput {
                    id: pill
                    anchors {
                        left: pillIcon.right
                        leftMargin: 10
                        right: parent.right
                        rightMargin: 16
                        verticalCenter: parent.verticalCenter
                    }
                    readOnly: root.saving
                    clip: true
                    selectByMouse: true
                    color: Theme.text
                    selectionColor: Theme.overlay2
                    selectedTextColor: Theme.textBright
                    font.family: Theme.fontMono
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    cursorDelegate: Rectangle {
                        width: 2
                        color: Theme.pistachio
                        visible: !root.saving
                    }
                    Accessible.name: root.saving ? "Folder" : "Filter files, or type a folder path"

                    Text {
                        visible: pill.text === ""
                        text: lister.displayPath(root.folder)
                        elide: Text.ElideMiddle
                        width: parent.width
                        color: Theme.text
                        font: pill.font
                    }

                    onTextEdited: root.filter = text
                    onAccepted: {
                        const typed = lister.folderFor(text);
                        if (typed !== "")
                            root.go(typed);
                        else
                            root.accept();
                    }
                    Keys.onUpPressed: root.move(-1)
                    Keys.onDownPressed: root.move(1)
                    Keys.onEscapePressed: (event) => {
                        event.accepted = text !== "";
                        if (text !== "") {
                            text = "";
                            root.filter = "";
                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: root.visibleRows * 44 + (root.visibleRows - 1) * 6 + 12
            radius: Theme.tileRadius
            color: Theme.bg

            ListView {
                id: list
                anchors.fill: parent
                anchors.margins: 6
                clip: true
                spacing: 6
                model: root.rows
                boundsBehavior: Flickable.StopAtBounds
                currentIndex: -1
                Controls.ScrollBar.vertical: ThinScrollBar {}

                delegate: Rectangle {
                    id: row

                    required property var modelData
                    required property int index
                    readonly property bool picked: ListView.isCurrentItem

                    width: ListView.view.width
                    height: 44
                    radius: 16
                    color: picked ? Theme.pistachio : hover.hovered ? Theme.overlay : "transparent"

                    Behavior on color {
                        ColorAnimation {
                            duration: Theme.hoverDuration
                            easing.type: Theme.easing
                        }
                    }

                    Icon {
                        x: 14
                        anchors.verticalCenter: parent.verticalCenter
                        size: 18
                        paths: row.modelData.parent ? Icons.chevronLeft : row.modelData.folder ? Icons.folder : Icons.file
                        color: row.picked ? Theme.bg : Theme.subtext
                    }

                    Text {
                        anchors {
                            left: parent.left
                            leftMargin: 44
                            right: info.left
                            rightMargin: 12
                            verticalCenter: parent.verticalCenter
                        }
                        text: row.modelData.name
                        textFormat: Text.PlainText
                        elide: Text.ElideMiddle
                        color: row.picked ? Theme.bg : Theme.text
                        font.family: Theme.fontUi
                        font.pixelSize: 13
                        font.weight: row.picked ? Font.ExtraBold : Font.Bold
                    }

                    Text {
                        id: info
                        anchors {
                            right: parent.right
                            rightMargin: 14
                            verticalCenter: parent.verticalCenter
                        }
                        text: row.modelData.size
                        color: row.picked ? Theme.bg : Theme.subtext
                        font.family: Theme.fontUi
                        font.pixelSize: 11
                        font.weight: row.picked ? Font.Bold : Font.DemiBold
                        font.features: { "tnum": 1 }
                    }

                    HoverHandler {
                        id: hover
                    }

                    TapHandler {
                        onTapped: list.currentIndex = row.index
                        onDoubleTapped: root.accept()
                    }
                }
            }
        }

        Item {
            width: parent.width
            height: 59

            Rectangle {
                width: parent.width
                height: 1
                color: Theme.overlay
            }

            Row {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                spacing: 10

                DialogButton {
                    text: "Cancel"
                    onClicked: root.close()
                }

                DialogButton {
                    text: root.saving ? "Save" : "Open"
                    primary: true
                    enabled: root.saving ? root.nameOk : list.currentIndex >= 0
                    onClicked: root.saving ? root.picked(root.folder, root.fileName) : root.accept()
                }
            }
        }
    }
}
