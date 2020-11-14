#pragma once

#include "notesview.h"
#include <QSystemTrayIcon>
#include <QWidget>

class MainWin : public QWidget {
    Q_OBJECT

public:
    MainWin();

private:
    QSystemTrayIcon *m_trayIcon;
    NotesView *m_notesView;
    bool m_needsRecalcGeometry;
    QRect m_trayGeo;

    virtual bool event(QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    void _gotoTrashFile();
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
    void _onIconActivated(QSystemTrayIcon::ActivationReason reason);
};
