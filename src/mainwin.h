
#ifndef MAINWIN_H
#define MAINWIN_H

#include "somenotes.h"
#include <QSystemTrayIcon>
#include <QWidget>

namespace Ui {
class About;
}

class MainWin : public QWidget {
    Q_OBJECT
public:
    explicit MainWin(QWidget *parent = nullptr);

signals:

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QSystemTrayIcon *m_trayIcon;
    bool m_needsRecalcGeometry;
    SomeNotes *m_someNotes;
    QRect m_trayGeo;

    void _openDataDir();
    void _about();
    void _appear();
    void _initTrayIcon();
    void _recalcGeometryIfNeeded();

#ifdef Q_OS_WIN
    QAction *m_autoStartAction;

    void _autoStartChanged();
    bool _isAutoStart();
    void _updateAutoStartIcon();
#endif

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