#pragma once

#include <QObject>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

// What the Open and Save dialogs show: one folder's entries, as the boards list them.
class FolderLister : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    using QObject::QObject;

    // Folders first, then files, each by name; every entry is { name, path, folder, size }. Dotfiles
    // only with `hidden`, files not at all with `foldersOnly`. An unreadable folder gives an empty list.
    Q_INVOKABLE QVariantList list(const QString &folder, bool hidden, bool foldersOnly) const;
    Q_INVOKABLE QString parentOf(const QString &folder) const;
    Q_INVOKABLE QString displayPath(const QString &path) const; // $HOME as ~
    // "~/x" as a full path; a path that isn't a folder gives ""
    Q_INVOKABLE QString folderFor(const QString &typed) const;
    Q_INVOKABLE bool exists(const QString &path) const;
};
