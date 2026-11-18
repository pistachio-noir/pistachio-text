project('pistachio-text', 'cpp', version: '0.1.0', default_options: ['cpp_std=c++20', 'buildtype=debugoptimized'])

if meson.get_compiler('cpp').get_id() != 'clang'
  error('Configure with CC=clang CXX=clang++')
endif

qt = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'Gui', 'Qml', 'Quick'], version: '>=6.9')
qt_test = dependency('qt6', modules: ['Test'])
syntax = dependency('KF6SyntaxHighlighting', method: 'cmake')
deps = [qt_dep, syntax]
inc = include_directories('src')
moc = qt.preprocess(moc_headers: files('src/Document.h', 'src/Highlighter.h', 'src/FolderLister.h', 'src/WheelZoom.h', 'src/SharedTheme.h'), dependencies: deps)
resources = qt.preprocess(qresources: 'meson/qml.qrc')
backend = static_library('ptx', files('src/Document.cpp', 'src/Highlighter.cpp', 'src/FolderLister.cpp', 'src/WheelZoom.cpp', 'src/qmlregistration.cpp'), moc, resources, include_directories: inc, dependencies: deps)
executable('pistachio-text', 'src/main.cpp', include_directories: inc, link_whole: backend, dependencies: deps, cpp_args: ['-DPTX_VERSION="@0@"'.format(meson.project_version())], install: true)
test_moc = qt.preprocess(moc_sources: 'tests/tst_backend.cpp', dependencies: [qt_dep, qt_test])
tst = executable('tst_backend', 'tests/tst_backend.cpp', test_moc, include_directories: inc, link_whole: backend, dependencies: [deps, qt_test])
test('backend', tst, env: ['QT_QPA_PLATFORM=offscreen'])
executable('ptx-shots', 'tests/shots.cpp', include_directories: inc, link_whole: backend, dependencies: [deps, qt_test])

entry = configuration_data()
entry.set('EXECUTABLE', get_option('prefix') / get_option('bindir') / 'pistachio-text')
configure_file(input: 'data/pistachio-text.desktop.in', output: 'pistachio-text.desktop', configuration: entry, install_dir: get_option('datadir') / 'applications')
install_data('data/pistachio-text.svg', install_dir: get_option('datadir') / 'icons/hicolor/scalable/apps')
