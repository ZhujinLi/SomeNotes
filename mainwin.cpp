#include "mainwin.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QMenu>

#define SETTING_WIDTH "width"
#define SETTING_HEIGHT "height"

MainWin::MainWin(QWidget *parent) : QWidget(parent)
{
    m_needsRecalcGeometry = false;

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    layout()->addWidget(scrollArea);

    m_qkNotes = new QkNotes(scrollArea);
    scrollArea->setWidget(m_qkNotes);

    _initTrayIcon();

    if (!g_settings.contains(SETTING_WIDTH) || !g_settings.contains(SETTING_HEIGHT)) {
        int w = m_trayIcon->geometry().width() * 10;
        setGeometry(0, 0, w, int(w * 1.5f));
    } else {
        setGeometry(0, 0, g_settings.value(SETTING_WIDTH).toInt(), g_settings.value(SETTING_HEIGHT).toInt());
    }

    m_needsRecalcGeometry = true;
}

void MainWin::_initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/images/tray.png"));

    QMenu* trayIconMenu = new QMenu(this);

    QAction* backupAction = new QAction(tr("&Backup"), this);
    connect(backupAction, &QAction::triggered, m_qkNotes, &QkNotes::backup);
    trayIconMenu->addAction(backupAction);

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWin::iconActivated);

    m_trayIcon->show();
}

void MainWin::_recalcGeometryIfNeeded()
{
    if (!m_needsRecalcGeometry)
        return;
    m_needsRecalcGeometry = false;

    QRect trayGeometry = m_trayIcon->geometry();
    int w = width();
    int h = height();

    bool isTrayAtTop = trayGeometry.y() == 0;
    if (isTrayAtTop) {
        setGeometry(trayGeometry.center().x() - w / 2,
                    trayGeometry.bottom() + trayGeometry.height() / 4,
                    w, h);
    } else {
        setGeometry(trayGeometry.center().x() - w / 2,
                    trayGeometry.top() - trayGeometry.height() / 4 - h,
                    w, h);
    }

    g_settings.setValue(SETTING_WIDTH, width());
    g_settings.setValue(SETTING_HEIGHT, height());
}

void MainWin::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    case Qt::Key_Minus:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() - 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Equal:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() + 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Underscore:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Plus:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() + 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case DRAG_KEY:
        QApplication::restoreOverrideCursor();
        break;
    default:
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void MainWin::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void MainWin::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        show();
        activateWindow();
        break;
    default:
        ;
    }
}

bool MainWin::event(QEvent *event)
{
    _recalcGeometryIfNeeded();
    return QWidget::event(event);
}

