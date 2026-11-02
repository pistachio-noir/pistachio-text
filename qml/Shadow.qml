import QtQuick
import QtQuick.Effects

// A board's drop shadow under a rounded panel: put it just before the panel, filling it, with the
// same radius.
RectangularShadow {
    // Theme.shadowPanel or Theme.shadowModal
    required property var kind

    offset.y: kind.y
    blur: kind.blur
    color: Qt.rgba(Theme.shadow.r, Theme.shadow.g, Theme.shadow.b, kind.alpha)
}
