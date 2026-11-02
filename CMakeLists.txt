cmake_minimum_required(VERSION 3.21)
project(pistachio-text VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Qml Quick Test)
find_package(KF6SyntaxHighlighting REQUIRED) # the language colours: the syntax-highlighting package
qt_standard_project_setup(REQUIRES 6.9)

# The backend (C++) and the UI (QML) as one static QML module, linked by the app, the tests and
# the screenshot tool. The QML files sit at the module's root (aliases), next to its qmldir, so
# they see each other and the singletons without imports.
set(PTX_QML
    Main.qml Theme.qml Icons.qml Icon.qml Shadow.qml
    IconButton.qml PillButton.qml DialogButton.qml ThinScrollBar.qml
    FileDialog.qml ConfirmDialog.qml QuickBar.qml
)
list(TRANSFORM PTX_QML PREPEND qml/ OUTPUT_VARIABLE PTX_QML_FILES)
foreach(file IN LISTS PTX_QML)
    set_source_files_properties(qml/${file} PROPERTIES QT_RESOURCE_ALIAS ${file})
endforeach()
set_source_files_properties(qml/Theme.qml qml/Icons.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_library(ptx STATIC)
qt_add_qml_module(ptx
    URI Pistachio.Text
    VERSION 1.0
    SOURCES
        src/Document.h src/Document.cpp
        src/Highlighter.h src/Highlighter.cpp
        src/FolderLister.h src/FolderLister.cpp
        src/WheelZoom.h src/WheelZoom.cpp
    QML_FILES ${PTX_QML_FILES}
)
target_include_directories(ptx PUBLIC src)
target_link_libraries(ptx PUBLIC Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick KF6::SyntaxHighlighting)

qt_add_executable(pistachio-text src/main.cpp)
target_compile_definitions(pistachio-text PRIVATE PTX_VERSION="${PROJECT_VERSION}")
target_link_libraries(pistachio-text PRIVATE ptx ptxplugin)

install(TARGETS pistachio-text RUNTIME DESTINATION bin)
# The desktop entry runs the app by its installed path, worked out at install time so that
# `cmake --install --prefix` counts: launchers don't search ~/.local/bin (the shell's PATH lacks it).
install(CODE "set(ptx_entry_in \"${CMAKE_CURRENT_SOURCE_DIR}/data/pistachio-text.desktop\")
set(ptx_entry_out \"${CMAKE_CURRENT_BINARY_DIR}/pistachio-text.desktop\")")
install(CODE [[
    file(READ "${ptx_entry_in}" entry)
    string(REGEX REPLACE "\n(Try)?Exec=pistachio-text" "\n\\1Exec=${CMAKE_INSTALL_PREFIX}/bin/pistachio-text" entry "${entry}")
    file(WRITE "${ptx_entry_out}" "${entry}")
]])
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pistachio-text.desktop DESTINATION share/applications)
# the app icon, in hicolor: the shell only finds hicolor icons (suite gotcha G-19)
install(FILES data/pistachio-text.svg DESTINATION share/icons/hicolor/scalable/apps)

# tests: the backend without a window, and the screenshot tool (ptx-shots OUTDIR)
enable_testing()

qt_add_executable(tst_backend tests/tst_backend.cpp)
target_link_libraries(tst_backend PRIVATE ptx Qt6::Test)
add_test(NAME backend COMMAND tst_backend)
set_tests_properties(backend PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

qt_add_executable(ptx-shots tests/shots.cpp)
target_link_libraries(ptx-shots PRIVATE ptx ptxplugin Qt6::Test)
