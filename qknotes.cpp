#include "pch.h"
#include "qknotes.h"
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QMenu>
#include <QScrollBar>

#define SETTING_WIDTH "width"
#define SETTING_HEIGHT "height"

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent)
{
    m_changeCount = 0;

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    _initTrayIcon();

    if (!g_settings.contains(SETTING_WIDTH) || !g_settings.contains(SETTING_HEIGHT)) {
        int w = m_trayIcon->geometry().width() * 10;
        setGeometry(0, 0, w, int(w * 1.5f));
    } else {
        setGeometry(0, 0, g_settings.value(SETTING_WIDTH).toInt(), g_settings.value(SETTING_HEIGHT).toInt());
    }

    m_noteBlock = new NoteBlock(this);
    m_noteBlock->setPlainText(m_mgr.getContent());
    connect(m_noteBlock, NoteBlock::textChanged, this, onNoteBlockTextChanged);

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
    _saveContent();
}

void QkNotes::onNoteBlockTextChanged()
{
    m_changeCount++;
    if (m_changeCount >= 50) {
        _saveContent();
        m_changeCount = 0;
    }
}

void QkNotes::_saveContent()
{
    m_mgr.setContent(m_noteBlock->toPlainText());

    m_mgr.saveIfNeeded();
    qInfo() << "Content saved.";
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
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            this->setGeometry(geometry().x(), geometry().y(), width() - 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Equal:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            this->setGeometry(geometry().x(), geometry().y(), width() + 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Underscore:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Plus:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() + 10);
            m_needsRecalcGeometry = true;
        }
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
    _saveContent();
    m_mgr.backup();
}

bool QkNotes::event(QEvent *event)
{
    _recalcGeometryIfNeeded();
    if (event->type() == QEvent::Wheel)
        if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
            const int STEP = 50;
            QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);
            auto hScrollBar = m_noteBlock->horizontalScrollBar();
            int value = hScrollBar->value()
                    + (wheelEvent->delta() < 0 ? STEP : -STEP);
            hScrollBar->setValue(value);
        }
    return QWidget::event(event);
}
