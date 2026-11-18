#pragma once
#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QStandardPaths>
#include <QtQml/qqmlregistration.h>

class SharedTheme : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool noir READ noir NOTIFY changed)
public:
    explicit SharedTheme(QObject *parent = nullptr) : QObject(parent) {
        connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this] { reload(); });
        connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this] { reload(); });
        reload();
    }
    bool noir() const { return m_noir; }
signals:
    void changed();
private:
    void reload() {
        const auto path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/pistachio/shared-theme";
        QFile file(path);
        const bool value = file.open(QIODevice::ReadOnly) && file.read(32).trimmed() == "noir";
        if (value != m_noir) { m_noir = value; emit changed(); }
        // Atomic replacements drop file watches; watch the nearest existing directory too.
        if (!m_watcher.files().isEmpty()) m_watcher.removePaths(m_watcher.files());
        if (!m_watcher.directories().isEmpty()) m_watcher.removePaths(m_watcher.directories());
        if (QFile::exists(path)) m_watcher.addPath(path);
        QString directory = QFileInfo(path).absolutePath();
        while (!QDir(directory).exists() && directory != "/")
            directory = QFileInfo(directory).absolutePath();
        m_watcher.addPath(directory);
    }
    QFileSystemWatcher m_watcher;
    bool m_noir = false;
};
