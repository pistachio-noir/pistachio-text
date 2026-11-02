import QtQuick
import QtQuick.Controls.Basic as Controls

// A slim overlay2 handle at the views' right edge, shown while scrolling or hovered, only when the
// content is taller than the view.
Controls.ScrollBar {
    id: root

    width: 10
    padding: 2
    policy: Controls.ScrollBar.AsNeeded
    minimumSize: 0.08

    background: Item {}

    contentItem: Rectangle {
        implicitWidth: 6
        radius: 3
        color: root.pressed ? Theme.subtext : Theme.overlay2
        opacity: root.size < 1 && (root.active || root.hovered) ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: Theme.motionDuration
                easing.type: Theme.easing
            }
        }
    }
}
