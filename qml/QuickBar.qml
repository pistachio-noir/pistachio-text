import QtQuick
import QtQuick.Layouts

// Not on the boards: one row under the toolbar, in the Open dialog's pill style, for Find (Ctrl+F)
// and Go to line (Ctrl+G). Enter submits (Shift+Enter: the previous match), Esc closes.
RowLayout {
    id: root

    property string mode: "find" // or "goto"
    property bool invalid // nothing found, no such line: the text turns terracotta
    readonly property alias text: field.text

    signal submitted(bool backwards)
    signal edited
    signal dismissed

    function open(prefill: string) {
        invalid = false;
        field.text = prefill;
        visible = true;
        field.forceActiveFocus();
        field.selectAll();
    }

    function dismiss() {
        visible = false;
        dismissed();
    }

    visible: false
    spacing: 10

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 44
        radius: 22
        color: Theme.bg

        Icon {
            id: glyph
            x: 16
            anchors.verticalCenter: parent.verticalCenter
            size: 18
            paths: root.mode === "find" ? Icons.search : Icons.hash
            color: Theme.subtext
        }

        TextInput {
            id: field
            anchors {
                left: glyph.right
                leftMargin: 10
                right: parent.right
                rightMargin: 16
                verticalCenter: parent.verticalCenter
            }
            clip: true
            selectByMouse: true
            color: root.invalid ? Theme.terracotta : Theme.textBright
            selectionColor: Theme.overlay2
            selectedTextColor: Theme.textBright
            font.family: Theme.fontMono
            font.pixelSize: 12
            font.weight: Font.Medium
            validator: root.mode === "goto" ? lineNumber : null
            cursorDelegate: Rectangle {
                width: 2
                color: Theme.pistachio
            }
            Accessible.name: root.mode === "find" ? "Find" : "Go to line"

            IntValidator {
                id: lineNumber
                bottom: 1
            }

            Text {
                visible: field.text === "" && !field.activeFocus
                text: root.mode === "find" ? "Find in file" : "Line number"
                color: Theme.subtext
                font: field.font
            }

            onTextEdited: {
                root.invalid = false;
                root.edited();
            }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Escape) {
                    root.dismiss();
                    event.accepted = true;
                } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    root.submitted(!!(event.modifiers & Qt.ShiftModifier));
                    event.accepted = true;
                }
            }
        }
    }

    IconButton {
        visible: root.mode === "find"
        icon: Icons.chevronUp
        label: "Previous match"
        onClicked: root.submitted(true)
    }

    IconButton {
        visible: root.mode === "find"
        icon: Icons.chevronDown
        label: "Next match"
        onClicked: root.submitted(false)
    }

    IconButton {
        icon: Icons.x
        label: "Close"
        onClicked: root.dismiss()
    }
}
