// Runs the real app on a made-up folder laid out like the design boards, takes the boards' three
// pictures and a few more, and tries keys on it: ptx-shots OUTDIR.
// Writes main.png, open.png, save.png, find.png, goto.png and confirm.png, and prints a PASS or
// FAIL line per check (exit code 1 on a FAIL). Offscreen (QT_QPA_PLATFORM=offscreen) it doesn't
// touch the desktop; the settings go to a temp folder.

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTemporaryDir>
#include <QTest>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(Pistachio_TextPlugin)

namespace {

int failures = 0;

bool check(bool ok, const char *what)
{
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    failures += !ok;
    return ok;
}

template<typename Predicate>
bool waitFor(Predicate done, int ms = 5000)
{
    QElapsedTimer timer;
    timer.start();
    while (!done() && timer.elapsed() < ms)
        QTest::qWait(20);
    return done();
}

void write(const QString &path, const QByteArray &bytes)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        qFatal("can't write %s", qPrintable(path));
    file.write(bytes);
}

// QTest::keyClicks takes a widget only (suite gotcha G-55)
void type(QWindow *window, const char *text)
{
    for (const char *c = text; *c; ++c)
        QTest::keyClick(window, *c);
}

QByteArray readAll(const QString &path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

QObject *object(QObject *from, const char *name)
{
    return from->property(name).value<QObject *>();
}

// the board's sample, with its line 6 holding the cursor
const char *sample =
    "use std::fs;\n\n// Count the lines in a file\nfn count_lines(path: &str) -> usize {\n"
    "    let text = fs::read_to_string(path).unwrap();\n    text.lines().count()\n}\n\n"
    "fn main() {\n    let n = count_lines(\"notes.txt\");\n    println!(\"{} lines\", n);\n}\n";

} // namespace

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: ptx-shots OUTDIR\n");
        return 2;
    }
    const QString out = QDir(QString::fromLocal8Bit(argv[1])).absolutePath();
    QDir().mkpath(out);

    QTemporaryDir config;
    qputenv("XDG_CONFIG_HOME", config.path().toLocal8Bit()); // the zoom and last folder, not yours
    QTemporaryDir tree;
    const QString notes = tree.path() + "/notes";
    QDir().mkpath(notes + "/drafts");
    QDir().mkpath(notes + "/snippets");
    write(notes + "/notes.txt", QByteArray(1200, 'n'));
    write(notes + "/count_lines.rs", sample);
    write(notes + "/todo.md", QByteArray(640, 't'));

    QGuiApplication app(argc, argv);
    app.setOrganizationName("pistachio-test");
    app.setApplicationName("pistachio-text-shots");
    QQmlApplicationEngine engine;
    engine.setInitialProperties({ { "startFile", notes + "/count_lines.rs" } });
    engine.loadFromModule("Pistachio.Text", "Main");
    auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().value(0));
    if (!window)
        return check(false, "the window loads"), 1;
    window->resize(912, 552);
    QObject *doc = object(window, "document");
    QObject *editor = object(window, "editor");
    QObject *highlighter = object(window, "highlighter");
    QObject *openDialog = object(window, "openDialog");
    QObject *saveDialog = object(window, "saveDialog");
    QObject *confirm = object(window, "confirmDialog");
    QObject *bar = object(window, "bar");
    const auto grab = [&](const char *name) {
        QTest::qWait(450); // popups fade in
        check(window->grabWindow().save(out + "/" + name), name);
    };
    const auto call = [&](QObject *on, const char *method) { QMetaObject::invokeMethod(on, method); };
    const auto visible = [](QObject *o) { return o->property("visible").toBool(); };
    const auto text = [&] { return editor->property("text").toString(); };

    // the Main board
    QTest::qWait(300);
    check(doc->property("name").toString() == "count_lines.rs", "the start file opens");
    check(highlighter->property("language").toString() == "Rust", "its language is Rust");
    check(text() == sample, "its text is all there");
    editor->setProperty("cursorPosition", text().indexOf("count()") + 7); // end of line 6
    editor->setProperty("focus", true);
    QTest::qWait(300);
    check(doc->property("line").toInt() == 6 && doc->property("column").toInt() == 25, "Ln 6, Col 25 as on the board");
    grab("main.png");

    check(std::abs(window->property("rowHeight").toDouble() * editor->property("lineCount").toInt() - editor->property("contentHeight").toDouble()) < 0.01,
        "the line numbers and the chip step as far as the text's lines do");
    // keys: Enter keeps the indent, Tab types four spaces, Ctrl+Z undoes, Ctrl+plus zooms
    QTest::keyClick(window, Qt::Key_Return);
    check(text().contains("count()\n    \n}"), "Enter keeps the line's indent");
    QTest::keyClick(window, Qt::Key_Tab);
    check(text().contains("count()\n        \n}"), "Tab types the indent unit");
    check(doc->property("modified").toBool(), "an edit shows as unsaved");
    const int size = window->property("fontSize").toInt();
    QTest::keyClick(window, Qt::Key_Equal, Qt::ControlModifier);
    check(window->property("fontSize").toInt() == size + 1, "Ctrl+plus zooms in");
    QTest::keyClick(window, Qt::Key_0, Qt::ControlModifier);
    check(window->property("fontSize").toInt() == 13, "Ctrl+0 resets the zoom");
    const auto wheel = [&](int delta, Qt::KeyboardModifiers modifiers) {
        QWheelEvent event(QPointF(400, 300), window->mapToGlobal(QPointF(400, 300)), {}, QPoint(0, delta), Qt::NoButton, modifiers, Qt::NoScrollPhase, false);
        QCoreApplication::sendEvent(window, &event);
    };
    wheel(120, Qt::ControlModifier);
    check(window->property("fontSize").toInt() == 14, "Ctrl+wheel up zooms in");
    wheel(-60, Qt::ControlModifier); // half a notch, as a trackpad sends
    check(window->property("fontSize").toInt() == 14, "half a notch waits");
    wheel(-60, Qt::ControlModifier);
    check(window->property("fontSize").toInt() == 13, "the other half completes the step");
    wheel(120, Qt::NoModifier);
    check(window->property("fontSize").toInt() == 13, "the wheel alone doesn't zoom");

    // unsaved text: opening another file asks first, and Cancel keeps it
    QMetaObject::invokeMethod(window, "openFile", Q_ARG(QString, notes + "/todo.md"));
    check(waitFor([&] { return visible(confirm); }), "opening over unsaved text asks first");
    grab("confirm.png");
    QTest::keyClick(window, Qt::Key_Return); // focus starts on Cancel
    check(waitFor([&] { return !visible(confirm); }) && doc->property("name").toString() == "count_lines.rs", "Enter on Cancel keeps the text");

    // the Open board
    call(window, "showOpen");
    check(waitFor([&] { return visible(openDialog); }), "Ctrl+O's card opens");
    QList<QVariant> rows = openDialog->property("rows").toList();
    check(rows.size() == 6 && rows.at(0).toMap().value("parent").toBool(), "Parent folder, two folders, three files");
    check(openDialog->property("fileName").toString().isEmpty() && rows.at(openDialog->property("visibleRows").toInt()).isNull() == false, "five rows show");
    check(rows.at(3).toMap().value("name").toString() == "count_lines.rs", "the open file is listed");
    grab("open.png");
    type(window, "todo"); // the pill filters
    rows = openDialog->property("rows").toList();
    check(rows.size() == 2 && rows.at(1).toMap().value("name").toString() == "todo.md", "typing filters the files");
    QTest::keyClick(window, Qt::Key_Escape);
    check(visible(openDialog) && openDialog->property("filter").toString().isEmpty(), "Esc clears the filter first");
    QTest::keyClick(window, Qt::Key_Escape);
    check(waitFor([&] { return !visible(openDialog); }), "then closes");

    // the Save as board
    call(window, "showSave");
    check(waitFor([&] { return visible(saveDialog); }), "Ctrl+Shift+S's card opens");
    rows = saveDialog->property("rows").toList();
    check(rows.size() == 3 && saveDialog->property("fileName").toString() == "count_lines.rs", "Save as: folders only, the file's name");
    grab("save.png");
    // saving over another file asks; saving under a new name doesn't
    QMetaObject::invokeMethod(window, "saveTo", Q_ARG(QString, notes), Q_ARG(QString, "todo.md"));
    check(waitFor([&] { return visible(confirm); }), "replacing a file asks first");
    QTest::keyClick(window, Qt::Key_Escape);
    check(waitFor([&] { return !visible(confirm); }) && QFile(notes + "/todo.md").size() == 640, "Esc leaves the other file alone");
    QMetaObject::invokeMethod(window, "saveTo", Q_ARG(QString, notes), Q_ARG(QString, "copy.rs"));
    check(QFile(notes + "/copy.rs").exists() && !doc->property("modified").toBool() && doc->property("name").toString() == "copy.rs", "Save as writes the file and renames the document");
    check(readAll(notes + "/copy.rs").contains("count()\n        \n}"), "with the edited text");
    call(saveDialog, "close");
    check(waitFor([&] { return !visible(saveDialog); }), "the card closes");

    // Find and Go to line
    call(window, "showFind");
    check(visible(bar), "Ctrl+F's row opens");
    type(window, "LINES");
    check(editor->property("selectedText").toString().toLower() == "lines", "typing selects the first match (any case)");
    QTest::keyClick(window, Qt::Key_Return);
    check(editor->property("selectionStart").toInt() > text().indexOf("lines"), "Enter goes to the next");
    grab("find.png");
    type(window, "zzz");
    check(bar->property("invalid").toBool(), "no match turns the text terracotta");
    QTest::keyClick(window, Qt::Key_Escape);
    check(!visible(bar), "Esc closes it");
    call(window, "showGoto");
    type(window, "10");
    grab("goto.png");
    QTest::keyClick(window, Qt::Key_Return);
    check(doc->property("line").toInt() == 10 && !visible(bar), "Go to line 10");

    // a file that isn't text says why
    write(tree.path() + "/bin", QByteArray("a\0b", 3));
    QMetaObject::invokeMethod(window, "load", Q_ARG(QString, tree.path() + "/bin"));
    check(!doc->property("error").toString().isEmpty() && doc->property("name").toString() == "copy.rs", "a binary file is refused, the text stays");

    // a file taller than the window: its Flickable takes the wheel, and Ctrl+wheel must still zoom
    editor->setProperty("text", QString("line\n").repeated(300));
    QTest::qWait(300);
    const int before = window->property("fontSize").toInt();
    QWheelEvent zoomIn(QPointF(400, 300), window->mapToGlobal(QPointF(400, 300)), {}, QPoint(0, 120), Qt::NoButton, Qt::ControlModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(window, &zoomIn);
    check(window->property("fontSize").toInt() == before + 1, "Ctrl+wheel zooms a long file too");
    QWheelEvent zoomOnNumbers(QPointF(40, 300), window->mapToGlobal(QPointF(40, 300)), {}, QPoint(0, -120), Qt::NoButton, Qt::ControlModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(window, &zoomOnNumbers);
    check(window->property("fontSize").toInt() == before, "and over the line numbers");

    std::printf("%d failure(s)\n", failures);
    return failures ? 1 : 0;
}
