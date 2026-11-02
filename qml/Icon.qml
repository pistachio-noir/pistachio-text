import QtQuick
import QtQuick.Shapes

// Lucide-style stroke icon from 24×24 SVG path data, as on the design boards.
// Each path must start with an absolute "M": the paths are joined into one.
Shape {
    id: root

    property list<string> paths
    property color color: Theme.text
    property real size: 18

    // explicit, not implicit: Shape resets its implicit size to the path bounds
    width: size
    height: size
    preferredRendererType: Shape.CurveRenderer

    Behavior on color {
        ColorAnimation {
            duration: Theme.hoverDuration
            easing.type: Theme.easing
        }
    }

    ShapePath {
        scale: Qt.size(root.size / 24, root.size / 24)
        strokeColor: root.color
        strokeWidth: 2 * root.size / 24
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        PathSvg {
            path: root.paths.join(" ")
        }
    }
}
