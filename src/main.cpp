#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(Pistachio_TextPlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("pistachio"));
    app.setApplicationName(QStringLiteral("pistachio-text"));
    app.setApplicationDisplayName(QStringLiteral("Text"));
    app.setApplicationVersion(QStringLiteral(PTX_VERSION));
    app.setDesktopFileName(QStringLiteral("pistachio-text")); // the Wayland app id, for window rules

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("A text and code editor in Pistachio Noir."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("file"),
        QStringLiteral("A file to open (path or file:// URL); one that doesn't exist yet is created by the first save."));
    parser.process(app);

    // xdg-open and FM hand over URLs, a terminal plain or relative paths
    QString start;
    if (const QStringList args = parser.positionalArguments(); !args.isEmpty()) {
        const QUrl url = QUrl::fromUserInput(args.first(), QDir::currentPath(), QUrl::AssumeLocalFile);
        if (url.isLocalFile())
            start = url.toLocalFile();
    }

    QQmlApplicationEngine engine;
    engine.setInitialProperties({ { QStringLiteral("startFile"), start } });
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.loadFromModule("Pistachio.Text", "Main");
    return app.exec();
}
