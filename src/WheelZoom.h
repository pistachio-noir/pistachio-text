#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

// Ctrl + wheel zooms. It watches the whole app's wheel events before Qt Quick hands them to an item,
// because WheelHandlers in the editor never saw the real mouse's Ctrl+wheel (simulated events
// reached them fine). One step per notch; a trackpad's small turns add up. PTX_DEBUG_WHEEL=1 logs
// every wheel event the window gets.
class WheelZoom : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)

public:
    explicit WheelZoom(QObject *parent = nullptr);

signals:
    void zoom(int steps);
    void enabledChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool m_enabled = true;
    int m_turn = 0;
};
