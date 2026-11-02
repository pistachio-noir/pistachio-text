#include "Highlighter.h"

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <QFileInfo>
#include <QTextDocument>

namespace {

KSyntaxHighlighting::Repository &repository()
{
    static KSyntaxHighlighting::Repository repo;
    return repo;
}

} // namespace

// The KSyntaxHighlighting side, with the colours read from its Highlighter.
class HighlightEngine : public KSyntaxHighlighting::SyntaxHighlighter
{
public:
    explicit HighlightEngine(Highlighter *owner)
        : KSyntaxHighlighting::SyntaxHighlighter(owner)
        , m_owner(owner)
    {
    }

protected:
    // shortcut: a line over 4000 characters (minified code) stays uncoloured, so typing in it stays quick
    void highlightBlock(const QString &text) override
    {
        if (text.size() <= 4000)
            KSyntaxHighlighting::SyntaxHighlighter::highlightBlock(text);
    }

    void applyFormat(int offset, int length, const KSyntaxHighlighting::Format &format) override;

private:
    Highlighter *m_owner;
};

Highlighter::Highlighter(QObject *parent)
    : QObject(parent)
    , m_engine(new HighlightEngine(this))
{
    // queued: the colours arrive while QML is still setting the object up
    connect(this, &Highlighter::colorsChanged, this, &Highlighter::rehighlight, Qt::QueuedConnection);
}

void Highlighter::rehighlight()
{
    m_engine->rehighlight();
}

void Highlighter::setTextDocument(QQuickTextDocument *document)
{
    if (document == m_textDocument)
        return;
    m_textDocument = document;
    m_engine->setDocument(document ? document->textDocument() : nullptr);
    emit textDocumentChanged();
}

void Highlighter::setPath(const QString &path)
{
    if (path == m_path)
        return;
    m_path = path;
    m_engine->setDefinition(path.isEmpty() ? KSyntaxHighlighting::Definition() : repository().definitionForFileName(QFileInfo(path).fileName()));
    emit languageChanged();
}

QString Highlighter::language() const
{
    const KSyntaxHighlighting::Definition definition = m_engine->definition();
    return definition.isValid() ? definition.name() : QStringLiteral("Plain Text");
}

void HighlightEngine::applyFormat(int offset, int length, const KSyntaxHighlighting::Format &format)
{
    using Style = KSyntaxHighlighting::Theme::TextStyle;
    const Highlighter &c = *m_owner;
    QTextCharFormat out;
    switch (format.textStyle()) {
    case Style::Keyword:
    case Style::ControlFlow:
    case Style::Import:
    case Style::Preprocessor:
        out.setForeground(c.m_keyword);
        break;
    case Style::Function:
    case Style::BuiltIn:
    case Style::Extension:
        out.setForeground(c.m_function);
        break;
    case Style::DataType:
    case Style::Constant:
    case Style::Attribute:
    case Style::Annotation:
        out.setForeground(c.m_type);
        break;
    case Style::String:
    case Style::Char:
    case Style::SpecialChar:
    case Style::VerbatimString:
    case Style::SpecialString:
        out.setForeground(c.m_string);
        break;
    case Style::DecVal:
    case Style::BaseN:
    case Style::Float:
        out.setForeground(c.m_number);
        break;
    case Style::Comment:
    case Style::Documentation:
    case Style::CommentVar:
        out.setForeground(c.m_comment);
        out.setFontItalic(true);
        break;
    default:
        return;
    }
    setFormat(offset, length, out);
}
