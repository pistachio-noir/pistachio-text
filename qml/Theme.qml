pragma Singleton

import QtQuick

// The only place with colours and sizes, in step with pistachio-fm. Values from design/canvas-source/*.dc.html.
QtObject {
    readonly property color bg: "#15170F"
    readonly property color bgAlt: "#1D2016"
    readonly property color overlay: "#2A2F1F"
    readonly property color overlay2: "#39412A"
    readonly property color muted: "#626F51" // ~3.4:1 on bg: never for small text
    readonly property color subtext: "#A3B08F"
    readonly property color text: "#E5E9D8"
    readonly property color textBright: "#F6F8EF"
    readonly property color pistachio: "#9FD88A" // active state only
    readonly property color terracotta: "#DB8267"
    readonly property color gold: "#E0B15E"
    readonly property color sageBlue: "#7DA3B3"
    readonly property color teal: "#7FC9A8"
    readonly property color lavender: "#AC8ED0"
    readonly property color clay: "#D9975A"
    // the boards' desktop (#0D0F09) at 66 %, behind dialogs
    readonly property color scrim: Qt.rgba(13 / 255, 15 / 255, 9 / 255, 0.66)

    readonly property int tileRadius: 20
    readonly property int panelRadius: 28

    // drop shadows from the boards (CSS box-shadow: 0 <y> <blur> black at <alpha>)
    readonly property color shadow: "#000000"
    readonly property var shadowPanel: ({ y: 18, blur: 44, alpha: 0.6 }) // menus
    readonly property var shadowModal: ({ y: 24, blur: 60, alpha: 0.65 }) // dialogs

    readonly property string fontUi: "Manrope"
    // code-like text; the Nerd Font build where plain JetBrains Mono isn't installed
    readonly property string fontMono: Qt.fontFamilies().indexOf("JetBrains Mono") >= 0 ? "JetBrains Mono" : "JetBrainsMono Nerd Font"

    // one easing curve for everything; CSS ease-out is close to OutCubic
    readonly property int easing: Easing.OutCubic
    readonly property int hoverDuration: 120
    readonly property int motionDuration: 200
}
