#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QtQml/qqmlregistration.h>

// The open file: loads it into the editor's text document and saves it back. Line endings (LF or
// CRLF) and the file's indent style are kept; text that isn't UTF-8, has NUL bytes or is over 10 MB
// is refused with `error` set and the document untouched.
class Document : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged REQUIRED)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QString name READ name NOTIFY pathChanged)
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)
    Q_PROPERTY(QString error MEMBER m_error NOTIFY errorChanged) // writable so the UI can clear it
    Q_PROPERTY(QString indentLabel READ indentLabel NOTIFY indentChanged)
    Q_PROPERTY(QString indentUnit READ indentUnit NOTIFY indentChanged)
    Q_PROPERTY(QString eolLabel READ eolLabel NOTIFY indentChanged)
    Q_PROPERTY(int cursorPosition MEMBER m_cursor WRITE setCursorPosition NOTIFY cursorChanged)
    Q_PROPERTY(int line READ line NOTIFY cursorChanged)
    Q_PROPERTY(int column READ column NOTIFY cursorChanged)

public:
    explicit Document(QObject *parent = nullptr);

    QQuickTextDocument *textDocument() const { return m_quickDocument; }
    void setTextDocument(QQuickTextDocument *document);
    QString path() const { return m_path; }
    QString name() const;
    bool modified() const;
    QString indentLabel() const;
    QString indentUnit() const { return m_tabs ? QStringLiteral("\t") : QString(m_width, u' '); }
    QString eolLabel() const { return m_crlf ? QStringLiteral("CRLF") : QStringLiteral("LF"); }
    void setCursorPosition(int position);
    int line() const;
    int column() const;

    // A path that doesn't exist yet opens empty, to be created by the first save.
    Q_INVOKABLE bool load(const QString &path);
    Q_INVOKABLE bool save(const QString &path);
    // The whitespace the line at `position` starts with, up to `position`: what Enter repeats.
    Q_INVOKABLE QString indentAt(int position) const;

signals:
    void textDocumentChanged();
    void pathChanged();
    void modifiedChanged();
    void errorChanged();
    void indentChanged();
    void cursorChanged();

private:
    bool fail(const QString &message);
    void detectIndent(const QString &text);

    QPointer<QQuickTextDocument> m_quickDocument;
    QString m_path;
    QString m_error;
    bool m_crlf = false;
    bool m_tabs = false;
    int m_width = 4;
    int m_cursor = 0;
};
