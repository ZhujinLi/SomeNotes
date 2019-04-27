#include "pch.h"
#include "qknotes.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QLayout>
#include <QMenu>

#define SETTING_WIDTH "width"
#define SETTING_HEIGHT "height"

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent)
{
    m_needsRecalcGeometry = false;

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    _initTrayIcon();

    if (!g_settings.contains(SETTING_WIDTH) || !g_settings.contains(SETTING_HEIGHT)) {
        int w = m_trayIcon->geometry().width() * 10;
        setGeometry(0, 0, w, int(w * 1.5f));
    } else {
        setGeometry(0, 0, g_settings.value(SETTING_WIDTH).toInt(), g_settings.value(SETTING_HEIGHT).toInt());
    }

    NoteBlockContent* content0 = m_mgr.getContentCount() ? m_mgr.getContent(0) : m_mgr.newContent();
    m_noteBlock = new NoteBlock(content0, this);
    m_noteBlock->setGeometry(0, 0, width(), 300);

    m_needsRecalcGeometry = true;
}

void QkNotes::_initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/images/tray.png"));

    QMenu* trayIconMenu = new QMenu(this);

    QAction* backupAction = new QAction(tr("&Backup"), this);
    connect(backupAction, &QAction::triggered, this, &QkNotes::backup);
    trayIconMenu->addAction(backupAction);

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &QkNotes::iconActivated);

    m_trayIcon->show();
}

QkNotes::~QkNotes()
{
    delete m_noteBlock; // since mgr is shared
}

void QkNotes::_recalcGeometryIfNeeded()
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

void QkNotes::keyReleaseEvent(QKeyEvent *event)
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
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void QkNotes::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void QkNotes::iconActivated(QSystemTrayIcon::ActivationReason reason)
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

void QkNotes::backup()
{
    m_noteBlock->saveContent();
    m_mgr.backup();
}

bool QkNotes::event(QEvent *event)
{
    _recalcGeometryIfNeeded();
    return QWidget::event(event);
}

