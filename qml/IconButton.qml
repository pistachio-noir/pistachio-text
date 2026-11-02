import QtQuick

// Round 44 px toolbar button with an 18 px icon. `active` marks the chosen view; a disabled button
// keeps its pill and dims its icon, like Forward on the boards.
Rectangle {
    id: root

    required property list<string> icon
    property string label
    property bool active
    property color restColor: Theme.text

    signal clicked

    implicitWidth: 44
    implicitHeight: 44
    radius: 22
    color: active || mouse.containsMouse ? Theme.overlay2 : Theme.overlay
    Accessible.role: Accessible.Button
    Accessible.name: label

    Behavior on color {
        ColorAnimation {
            duration: Theme.hoverDuration
            easing.type: Theme.easing
        }
    }

    Icon {
        anchors.centerIn: parent
        size: 18
        paths: root.icon
        color: !root.enabled ? Theme.subtext : root.active || mouse.containsMouse ? Theme.textBright : root.restColor
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
