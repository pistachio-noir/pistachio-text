#pragma once

#include <QColor>
#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QtQml/qqmlregistration.h>

class HighlightEngine;

// Syntax colours from KSyntaxHighlighting, mapped onto the boards' few: keyword, function, type,
// string, number, comment. The language comes from the file name. The colours come from QML (Theme).
class Highlighter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY languageChanged)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
    Q_PROPERTY(QColor keywordColor MEMBER m_keyword NOTIFY colorsChanged)
    Q_PROPERTY(QColor functionColor MEMBER m_function NOTIFY colorsChanged)
    Q_PROPERTY(QColor typeColor MEMBER m_type NOTIFY colorsChanged)
    Q_PROPERTY(QColor stringColor MEMBER m_string NOTIFY colorsChanged)
    Q_PROPERTY(QColor numberColor MEMBER m_number NOTIFY colorsChanged)
    Q_PROPERTY(QColor commentColor MEMBER m_comment NOTIFY colorsChanged)

public:
    explicit Highlighter(QObject *parent = nullptr);

    QQuickTextDocument *textDocument() const { return m_textDocument; }
    void setTextDocument(QQuickTextDocument *document);
    QString path() const { return m_path; }
    void setPath(const QString &path);
    QString language() const;
    void rehighlight();

signals:
    void textDocumentChanged();
    void languageChanged();
    void colorsChanged();

private:
    friend class HighlightEngine;

    HighlightEngine *m_engine;
    QPointer<QQuickTextDocument> m_textDocument;
    QString m_path;
    QColor m_keyword, m_function, m_type, m_string, m_number, m_comment;
};
