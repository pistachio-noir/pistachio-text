import QtQuick

// A toolbar button as on the Main board: 44 px pill with an icon, a label and its key in mono.
Rectangle {
    id: root

    required property list<string> icon
    property string text
    property string hint

    signal clicked

    implicitWidth: row.implicitWidth + 36
    implicitHeight: 44
    radius: Theme.tileRadius
    color: mouse.containsMouse ? Theme.overlay2 : Theme.overlay
    Accessible.role: Accessible.Button
    Accessible.name: text

    Behavior on color {
        ColorAnimation {
            duration: Theme.hoverDuration
            easing.type: Theme.easing
        }
    }

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 10

        Icon {
            anchors.verticalCenter: parent.verticalCenter
            size: 18
            paths: root.icon
            color: Theme.text
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.text
            textFormat: Text.PlainText
            color: Theme.text
            font.family: Theme.fontUi
            font.pixelSize: 13
            font.weight: Font.Bold
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.hint
            textFormat: Text.PlainText
            color: Theme.subtext
            font.family: Theme.fontMono
            font.pixelSize: 11
            font.weight: Font.Medium
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
