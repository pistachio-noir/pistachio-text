#include "FolderLister.h"

#include <QDir>
#include <QFileInfo>

namespace {

// the boards' sizes: "286 B", "1.2 KB", "14 MB"
QString sizeText(qint64 bytes)
{
    if (bytes < 1000)
        return QStringLiteral("%1 B").arg(bytes);
    static const char *const units[] = { "KB", "MB", "GB", "TB" };
    double value = bytes / 1000.0;
    int unit = 0;
    while (unit < 3 && value >= 1000) {
        value /= 1000;
        ++unit;
    }
    return QStringLiteral("%1 %2").arg(value < 10 ? QString::number(value, 'f', 1) : QString::number(qRound(value)), QLatin1String(units[unit]));
}

} // namespace

QVariantList FolderLister::list(const QString &folder, bool hidden, bool foldersOnly) const
{
    QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable;
    if (!foldersOnly)
        filters |= QDir::Files;
    if (hidden)
        filters |= QDir::Hidden;
    QVariantList entries;
    for (const QFileInfo &info : QDir(folder).entryInfoList(filters, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase | QDir::LocaleAware)) {
        entries.append(QVariantMap {
            { QStringLiteral("name"), info.fileName() },
            { QStringLiteral("path"), info.absoluteFilePath() },
            { QStringLiteral("folder"), info.isDir() },
            { QStringLiteral("size"), info.isDir() ? QStringLiteral("Folder") : sizeText(info.size()) },
        });
    }
    return entries;
}

QString FolderLister::parentOf(const QString &folder) const
{
    QDir dir(folder);
    return dir.cdUp() ? dir.absolutePath() : folder;
}

QString FolderLister::displayPath(const QString &path) const
{
    const QString home = QDir::homePath();
    if (path == home)
        return QStringLiteral("~");
    return path.startsWith(home + u'/') ? QLatin1Char('~') + path.mid(home.size()) : path;
}

QString FolderLister::folderFor(const QString &typed) const
{
    QString path = typed.trimmed();
    if (path == QLatin1String("~") || path.startsWith(QLatin1String("~/")))
        path = QDir::homePath() + path.mid(1);
    const QFileInfo info(path);
    return info.isAbsolute() && info.isDir() ? QDir::cleanPath(path) : QString();
}

bool FolderLister::exists(const QString &path) const
{
    return QFileInfo::exists(path);
}
