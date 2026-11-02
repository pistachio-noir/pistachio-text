// The backend without a window: Document, FolderLister and Highlighter on temp files.
// QT_QPA_PLATFORM=offscreen ./tst_backend

#include "Document.h"
#include "FolderLister.h"
#include "Highlighter.h"

#include <QDir>
#include <QFile>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QtTest>

namespace {

void write(const QString &path, const QByteArray &bytes)
{
    QFile file(path);
    QVERIFY2(file.open(QIODevice::WriteOnly), qPrintable(path));
    file.write(bytes);
}

QByteArray read(const QString &path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray("<unreadable>");
}

} // namespace

class TestBackend : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QQmlComponent component(&m_engine);
        component.setData("import QtQuick\nTextEdit {}", QUrl());
        m_edit.reset(component.create());
        QVERIFY(m_edit);
        m_quick = m_edit->property("textDocument").value<QQuickTextDocument *>();
        QVERIFY(m_quick);
    }

    void roundTrip()
    {
        QTemporaryDir dir;
        const QString in = dir.filePath("a.txt"), out = dir.filePath("b.txt");
        write(in, "héllo\nworld\n");
        Document doc;
        doc.setTextDocument(m_quick);
        QVERIFY(doc.load(in));
        QCOMPARE(m_quick->textDocument()->toPlainText(), QString("héllo\nworld\n"));
        QVERIFY(!doc.modified());
        QCOMPARE(doc.name(), QString("a.txt"));

        QTextCursor(m_quick->textDocument()).insertText("x");
        QVERIFY(doc.modified());
        QVERIFY(doc.save(out));
        QVERIFY(!doc.modified());
        QCOMPARE(doc.path(), out);
        QCOMPARE(read(out), QByteArray("xhéllo\nworld\n"));
    }

    void crlfKept()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("w.txt");
        write(path, "a\r\nb\r\n");
        Document doc;
        doc.setTextDocument(m_quick);
        QVERIFY(doc.load(path));
        QCOMPARE(m_quick->textDocument()->toPlainText(), QString("a\nb\n"));
        QCOMPARE(doc.eolLabel(), QString("CRLF"));
        QVERIFY(doc.save(path));
        QCOMPARE(read(path), QByteArray("a\r\nb\r\n"));
    }

    void refusesWhatIsntText()
    {
        QTemporaryDir dir;
        Document doc;
        doc.setTextDocument(m_quick);
        const QString good = dir.filePath("good.txt");
        write(good, "keep me");
        QVERIFY(doc.load(good));

        const QString binary = dir.filePath("bin"), latin = dir.filePath("latin"), big = dir.filePath("big");
        write(binary, QByteArray("a\0b", 3));
        write(latin, "caf\xe9");
        QFile file(big);
        QVERIFY(file.open(QIODevice::WriteOnly) && file.resize(10 * 1000 * 1000 + 1)); // sparse
        file.close();
        for (const QString &path : { binary, latin, big }) {
            QVERIFY2(!doc.load(path), qPrintable(path));
            QVERIFY(!doc.property("error").toString().isEmpty());
            QCOMPARE(doc.path(), good); // the open file is untouched
            QCOMPARE(m_quick->textDocument()->toPlainText(), QString("keep me"));
        }
    }

    void missingFileOpensEmpty()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("new.txt");
        Document doc;
        doc.setTextDocument(m_quick);
        QVERIFY(doc.load(path));
        QVERIFY(m_quick->textDocument()->isEmpty());
        QCOMPARE(doc.path(), path);
        QTextCursor(m_quick->textDocument()).insertText("hi");
        QVERIFY(doc.save(path));
        QCOMPARE(read(path), QByteArray("hi"));
    }

    void failedSaveKeepsTheText()
    {
        Document doc;
        doc.setTextDocument(m_quick);
        QTextCursor(m_quick->textDocument()).insertText("x");
        QVERIFY(!doc.save("/proc/nope/file.txt"));
        QVERIFY(!doc.property("error").toString().isEmpty());
        QVERIFY(doc.modified());
    }

    void indentIsDetected()
    {
        QTemporaryDir dir;
        Document doc;
        doc.setTextDocument(m_quick);
        const struct { const char *text, *label, *unit; } cases[] = {
            { "fn a() {\n  x;\n  if y {\n    z;\n  }\n}\n", "Spaces: 2", "  " },
            { "fn a() {\n\tx;\n\ty;\n}\n", "Tabs", "\t" },
            { "plain\nlines\n", "Spaces: 4", "    " },
        };
        for (const auto &c : cases) {
            const QString path = dir.filePath("f");
            write(path, c.text);
            QVERIFY(doc.load(path));
            QCOMPARE(doc.indentLabel(), QString(c.label));
            QCOMPARE(doc.indentUnit(), QString(c.unit));
        }
    }

    void positions()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("f");
        write(path, "  ab\ncd\n");
        Document doc;
        doc.setTextDocument(m_quick);
        QVERIFY(doc.load(path));
        QCOMPARE(doc.indentAt(4), QString("  ")); // end of line 1
        QCOMPARE(doc.indentAt(1), QString(" ")); // only what's before the cursor
        QCOMPARE(doc.indentAt(6), QString()); // line 2
        doc.setCursorPosition(6);
        QCOMPARE(doc.line(), 2);
        QCOMPARE(doc.column(), 2);
        doc.setCursorPosition(0);
        QCOMPARE(doc.line(), 1);
        QCOMPARE(doc.column(), 1);
    }

    void folderListing()
    {
        QTemporaryDir dir;
        const QString root = dir.path();
        QVERIFY(QDir(root).mkdir("b") && QDir(root).mkdir("A"));
        write(root + "/z.txt", "z");
        write(root + "/a.txt", QByteArray(286, 'x'));
        write(root + "/.hidden", "h");
        FolderLister lister;

        const auto names = [&](bool hidden, bool foldersOnly) {
            QStringList out;
            for (const QVariant &entry : lister.list(root, hidden, foldersOnly))
                out << entry.toMap().value("name").toString();
            return out;
        };
        QCOMPARE(names(false, false), QStringList({ "A", "b", "a.txt", "z.txt" }));
        QCOMPARE(names(true, false), QStringList({ "A", "b", ".hidden", "a.txt", "z.txt" }));
        QCOMPARE(names(false, true), QStringList({ "A", "b" }));
        QCOMPARE(lister.list(root, false, false).at(2).toMap().value("size").toString(), QString("286 B"));
        QCOMPARE(lister.list(root, false, false).at(0).toMap().value("size").toString(), QString("Folder"));
        QVERIFY(lister.list(root + "/nope", false, false).isEmpty());

        QCOMPARE(lister.parentOf(root + "/A"), root);
        QCOMPARE(lister.parentOf("/"), QString("/"));
        QCOMPARE(lister.displayPath(QDir::homePath() + "/x"), QString("~/x"));
        QCOMPARE(lister.displayPath("/etc"), QString("/etc"));
        QCOMPARE(lister.folderFor("~"), QDir::homePath());
        QCOMPARE(lister.folderFor(root + "/A"), root + "/A");
        QCOMPARE(lister.folderFor(root + "/a.txt"), QString()); // a file, not a folder
        QCOMPARE(lister.folderFor("relative"), QString());
    }

    void highlighterNamesTheLanguageAndColoursIt()
    {
        QTextDocument *doc = m_quick->textDocument();
        doc->setPlainText("fn main() { let s = \"hi\"; } // c");
        Highlighter highlighter;
        highlighter.setProperty("keywordColor", QColor("#AC8ED0"));
        highlighter.setProperty("stringColor", QColor("#7FC9A8"));
        highlighter.setProperty("commentColor", QColor("#A3B08F"));
        highlighter.setTextDocument(m_quick);
        QCOMPARE(highlighter.language(), QString("Plain Text"));
        highlighter.setPath("/x/count_lines.rs");
        QCOMPARE(highlighter.language(), QString("Rust"));
        highlighter.setPath("/x/main.cpp");
        QCOMPARE(highlighter.language(), QString("C++"));
        highlighter.setPath("/x/count_lines.rs");
        highlighter.rehighlight(); // the colours land on the next event-loop turn (G-67)

        const auto colourAt = [&](int at) {
            for (const QTextLayout::FormatRange &range : doc->firstBlock().layout()->formats())
                if (at >= range.start && at < range.start + range.length)
                    return range.format.foreground().color();
            return QColor();
        };
        QCOMPARE(colourAt(0), QColor("#AC8ED0")); // fn
        QCOMPARE(colourAt(21), QColor("#7FC9A8")); // "hi"
        QCOMPARE(colourAt(31), QColor("#A3B08F")); // // c
        QVERIFY(!colourAt(3).isValid()); // main: plain text
    }

private:
    QQmlEngine m_engine;
    std::unique_ptr<QObject> m_edit;
    QQuickTextDocument *m_quick = nullptr;
};

QTEST_MAIN(TestBackend)
#include "tst_backend.moc"
