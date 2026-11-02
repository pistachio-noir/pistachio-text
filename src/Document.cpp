#include "Document.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringDecoder>
#include <QTextBlock>
#include <QTextDocument>

namespace {
constexpr qint64 MaxBytes = 10 * 1000 * 1000;
}

Document::Document(QObject *parent)
    : QObject(parent)
{
}

void Document::setTextDocument(QQuickTextDocument *document)
{
    if (document == m_quickDocument)
        return;
    m_quickDocument = document;
    if (document)
        connect(document->textDocument(), &QTextDocument::modificationChanged, this, &Document::modifiedChanged);
    emit textDocumentChanged();
}

QString Document::name() const
{
    return m_path.isEmpty() ? QStringLiteral("Untitled") : QFileInfo(m_path).fileName();
}

bool Document::modified() const
{
    return m_quickDocument && m_quickDocument->textDocument()->isModified();
}

QString Document::indentLabel() const
{
    return m_tabs ? QStringLiteral("Tabs") : QStringLiteral("Spaces: %1").arg(m_width);
}

void Document::setCursorPosition(int position)
{
    m_cursor = position;
    emit cursorChanged();
}

int Document::line() const
{
    return m_quickDocument ? m_quickDocument->textDocument()->findBlock(m_cursor).blockNumber() + 1 : 1;
}

int Document::column() const
{
    return m_quickDocument ? m_cursor - m_quickDocument->textDocument()->findBlock(m_cursor).position() + 1 : 1;
}

bool Document::fail(const QString &message)
{
    m_error = message;
    emit errorChanged();
    return false;
}

bool Document::load(const QString &path)
{
    QTextDocument *document = m_quickDocument ? m_quickDocument->textDocument() : nullptr;
    if (!document)
        return false;
    const QString name = QFileInfo(path).fileName();
    QString text;
    bool crlf = false;
    if (QFileInfo::exists(path)) {
        QFile file(path);
        if (QFileInfo(path).size() > MaxBytes)
            return fail(QStringLiteral("“%1” is over 10 MB.").arg(name));
        if (!file.open(QIODevice::ReadOnly))
            return fail(QStringLiteral("Can't read “%1”: %2").arg(name, file.errorString()));
        const QByteArray bytes = file.readAll();
        QStringDecoder decoder(QStringDecoder::Utf8, QStringConverter::Flag::Stateless); // stateless: a cut-off last character is an error too; keeps a BOM as a character, so a save writes it back
        text = decoder.decode(bytes);
        if (bytes.contains('\0') || decoder.hasError())
            return fail(QStringLiteral("“%1” isn't a UTF-8 text file.").arg(name));
        crlf = text.contains(QLatin1String("\r\n"));
        if (crlf)
            text.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    }

    m_error.clear();
    m_crlf = crlf;
    detectIndent(text);
    document->setPlainText(text);
    document->clearUndoRedoStacks();
    document->setModified(false);
    m_path = path;
    emit pathChanged();
    emit errorChanged();
    emit indentChanged();
    emit modifiedChanged();
    emit cursorChanged(); // the old position, in the new text
    return true;
}

bool Document::save(const QString &path)
{
    if (!m_quickDocument)
        return false;
    QTextDocument *document = m_quickDocument->textDocument();
    // shortcut: toPlainText turns U+00A0 into a plain space, so a no-break space is lost on save; upgrade if that matters
    QString text = document->toPlainText();
    if (m_crlf)
        text.replace(u'\n', QLatin1String("\r\n"));
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(text.toUtf8()) < 0 || !file.commit())
        return fail(QStringLiteral("Can't save “%1”: %2").arg(QFileInfo(path).fileName(), file.errorString()));

    m_error.clear();
    document->setModified(false);
    if (path != m_path) {
        m_path = path;
        emit pathChanged();
    }
    emit errorChanged();
    emit modifiedChanged();
    return true;
}

QString Document::indentAt(int position) const
{
    if (!m_quickDocument)
        return {};
    const QTextBlock block = m_quickDocument->textDocument()->findBlock(position);
    const QString text = block.text().left(position - block.position());
    qsizetype n = 0;
    while (n < text.size() && (text[n] == u' ' || text[n] == u'\t'))
        ++n;
    return text.left(n);
}

// shortcut: tabs if more of the first 2000 lines start with one than with spaces; the width is the smallest
// run of 2 to 8 leading spaces, else 4. A file with odd continuation lines may show a wrong width.
void Document::detectIndent(const QString &text)
{
    int tabLines = 0;
    int spaceLines = 0;
    int width = 0;
    qsizetype at = 0;
    for (int i = 0; i < 2000 && at < text.size(); ++i) {
        const qsizetype end = text.indexOf(u'\n', at) < 0 ? text.size() : text.indexOf(u'\n', at);
        if (text[at] == u'\t') {
            ++tabLines;
        } else if (text[at] == u' ') {
            ++spaceLines;
            qsizetype run = 0;
            while (at + run < end && text[at + run] == u' ')
                ++run;
            if (at + run < end && run >= 2 && run <= 8 && (width == 0 || run < width))
                width = int(run);
        }
        at = end + 1;
    }
    m_tabs = tabLines > spaceLines;
    m_width = width ? width : 4;
}
