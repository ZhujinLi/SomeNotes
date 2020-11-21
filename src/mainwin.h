#pragma once

#include "notesview.h"
#include <QSystemTrayIcon>
#include <QWidget>

enum class ViewMode {
    Portrait = 0,
    Landscape,
};

class MainWin : public QWidget {
    Q_OBJECT

public:
    MainWin();

private:
    QSystemTrayIcon *m_trayIcon;
    NotesView *m_notesView;
    bool m_needsRecalcGeometry;
    QRect m_trayGeo;
    QAction *m_portraitAction;
    QAction *m_landscapeAction;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    void _gotoTrashFile();
    void _about();
    void _appear();
    void _initTrayIcon();
    void _recalcGeometryIfNeeded();

    void _onPortraitModeSelected();
    void _onLandscapeModeSelected();
    void _setViewMode(ViewMode viewMode);

#ifdef Q_OS_WIN
    QAction *m_autoStartAction;

    void _autoStartChanged();
    bool _isAutoStart();
    void _updateAutoStartIcon();
#endif

private slots:
    void _onIconActivated(QSystemTrayIcon::ActivationReason reason);
};
