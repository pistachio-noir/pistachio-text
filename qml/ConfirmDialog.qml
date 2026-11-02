import QtQuick
import QtQuick.Controls.Basic as Controls

// Not on the boards: a card in the Open dialog's style, for what can't be undone (dropping unsaved
// text, replacing a file). Focus starts on Cancel, so Enter can't do it by accident.
// ask() takes { title, description, action, run }.
Controls.Popup {
    id: root

    property string title
    property string description
    property string action
    property var run: null

    function ask(options) {
        title = options.title;
        description = options.description;
        action = options.action;
        run = options.run;
        open();
    }

    parent: Controls.Overlay.overlay
    anchors.centerIn: parent
    width: 440
    padding: 28
    modal: true
    focus: true
    closePolicy: Controls.Popup.CloseOnEscape | Controls.Popup.CloseOnPressOutside

    onOpened: cancel.forceActiveFocus()

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

        Rectangle {
            width: 56
            height: 56
            radius: 28
            color: Theme.overlay

            Icon {
                anchors.centerIn: parent
                size: 24
                paths: Icons.alert
                color: Theme.terracotta
            }
        }

        Column {
            width: parent.width
            spacing: 6

            Text {
                width: parent.width
                text: root.title
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: Theme.textBright
                font.family: Theme.fontUi
                font.pixelSize: 18
                font.weight: Font.ExtraBold
            }

            Text {
                width: parent.width
                text: root.description
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                lineHeight: 17
                lineHeightMode: Text.FixedHeight
                color: Theme.subtext
                font.family: Theme.fontUi
                font.pixelSize: 12
                font.weight: Font.Medium
            }
        }

        Row {
            anchors.right: parent.right
            topPadding: 4
            spacing: 10

            DialogButton {
                id: cancel
                text: "Cancel"
                onClicked: root.close()
            }

            DialogButton {
                text: root.action
                textColor: Theme.terracotta
                strong: true
                onClicked: {
                    const run = root.run;
                    root.close();
                    run();
                }
            }
        }
    }
}
