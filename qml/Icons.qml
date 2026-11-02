pragma Singleton

import QtQuick

// Lucide path data from the design boards, for Icon.paths. Every path starts with an absolute "M".
QtObject {
    readonly property list<string> file: ["M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z", "M14 2v6h6"]
    readonly property list<string> folder: ["M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z"]
    readonly property list<string> folderOpen: ["M6 14l1.5-2.9A2 2 0 0 1 9.24 10H20a2 2 0 0 1 1.94 2.5l-1.54 6a2 2 0 0 1-1.95 1.5H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h3.9a2 2 0 0 1 1.69.9l.81 1.2a2 2 0 0 0 1.67.9H18a2 2 0 0 1 2 2v2"]
    readonly property list<string> save: ["M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z", "M17 21v-8H7v8", "M7 3v5h8"]
    readonly property list<string> chevronLeft: ["M15 18l-6-6 6-6"]
    readonly property list<string> chevronUp: ["M18 15l-6-6-6 6"]
    readonly property list<string> chevronDown: ["M6 9l6 6 6-6"]
    readonly property list<string> search: ["M3 11a8 8 0 1 0 16 0a8 8 0 1 0-16 0", "M21 21l-4.3-4.3"]
    readonly property list<string> x: ["M18 6L6 18", "M6 6l12 12"]
    readonly property list<string> alert: ["M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z", "M12 9v4", "M12 17h.01"]
    readonly property list<string> hash: ["M4 9h16", "M4 15h16", "M10 3L8 21", "M16 3l-2 18"]
}
