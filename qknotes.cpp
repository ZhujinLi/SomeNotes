#include "qknotes.h"
#include "ui_qknotes.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QMenu>
#include <QScrollBar>
#include <fstream>

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkNotes)
{
    m_changeCount = 0;

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    _initTrayIcon();

    m_size.setWidth(m_trayIcon->geometry().width() * 10);
    m_size.setHeight(int(m_size.width() * 1.5f));

    ui->noteEdit->setPlainText(m_mgr.getContent().c_str());

    m_needsRecalcGeometry = true;
}

void QkNotes::_initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/images/tray.png"));

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    QMenu* trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quitAction);
    m_trayIcon->setContextMenu(trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &QkNotes::iconActivated);

    m_trayIcon->show();
}

QkNotes::~QkNotes()
{
    _saveContent();
    delete ui;
}

void QkNotes::on_textEdit_textChanged()
{
    m_changeCount++;
    if (m_changeCount >= 50) {
        _saveContent();
        m_changeCount = 0;
    }
}

void QkNotes::_saveContent()
{
    m_mgr.setContent(ui->noteEdit->toPlainText().toStdString());

    m_mgr.saveIfNeeded();
    qInfo() << "Content saved.";
}

void QkNotes::_recalcGeometryIfNeeded()
{
    if (!m_needsRecalcGeometry)
        return;
    m_needsRecalcGeometry = false;

    QRect trayGeometry = m_trayIcon->geometry();
    int w = m_size.width();
    int h = m_size.height();

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
}

void QkNotes::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    case Qt::Key_Minus:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_size.setWidth(m_size.width() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Equal:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_size.setWidth(m_size.width() + 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Underscore:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_size.setHeight(m_size.height() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Plus:
        if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_size.setHeight(m_size.height() + 10);
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
        break;
    default:
        ;
    }
}

bool QkNotes::event(QEvent *event)
{
    _recalcGeometryIfNeeded();
    if (event->type() == QEvent::Wheel)
        if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
            const int STEP = 50;
            QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);
            auto hScrollBar = ui->noteEdit->horizontalScrollBar();
            int value = hScrollBar->value()
                    + (wheelEvent->delta() < 0 ? STEP : -STEP);
            hScrollBar->setValue(value);
        }
    return QWidget::event(event);
}
