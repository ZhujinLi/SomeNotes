
#include "qknotes.h"
#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QSystemTrayIcon>

namespace Ui {
class About;
}

class MainWin : public QWidget
{
    Q_OBJECT
public:
    explicit MainWin(QWidget *parent = nullptr);

signals:

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QSystemTrayIcon* m_trayIcon;
    bool m_needsRecalcGeometry;
    QkNotes* m_qkNotes;
    QRect m_trayGeo;

    void _openDataDir();
    void _about();
    void _restart();
    void _appear();
    void _initTrayIcon();
    void _recalcGeometryIfNeeded();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // MAINWIN_H
