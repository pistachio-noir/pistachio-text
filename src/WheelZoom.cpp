#include "WheelZoom.h"

#include <QDebug>
#include <QGuiApplication>
#include <QWheelEvent>

WheelZoom::WheelZoom(QObject *parent)
    : QObject(parent)
{
    qApp->installEventFilter(this);
}

bool WheelZoom::eventFilter(QObject *watched, QEvent *event)
{
    // only the window's own copy: Qt Quick sends the same event on to each item under the pointer
    if (event->type() != QEvent::Wheel || !watched->isWindowType())
        return false;
    auto *wheel = static_cast<QWheelEvent *>(event);
    static const bool debug = qEnvironmentVariableIsSet("PTX_DEBUG_WHEEL");
    if (debug)
        qInfo() << "wheel" << wheel->modifiers() << "keyboard" << QGuiApplication::queryKeyboardModifiers()
                << "angle" << wheel->angleDelta() << "pixel" << wheel->pixelDelta() << wheel->phase()
                << wheel->pointingDevice()->type() << "enabled" << m_enabled;
    if (!m_enabled || !(wheel->modifiers() & Qt::ControlModifier))
        return false;

    m_turn += wheel->angleDelta().y() != 0 ? wheel->angleDelta().y() : wheel->pixelDelta().y() * 8;
    const int steps = m_turn / 120;
    m_turn -= steps * 120;
    if (steps != 0)
        emit zoom(steps);
    return true; // Ctrl + wheel never scrolls the text
}
