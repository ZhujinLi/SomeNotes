#include "qknotes.h"
#include "ui_qknotes.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QMenu>
#include <QScrollBar>
#include <fstream>

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent, Qt::Dialog),
    ui(new Ui::QkNotes)
{
    m_changeCount = 0;

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    _initTrayIcon();

    ui->textEdit->setText(m_mgr.getContent().c_str());
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
    m_mgr.setContent(ui->textEdit->toPlainText().toStdString());

    m_mgr.saveIfNeeded();
    qInfo() << "Content saved.";
}

void QkNotes::_setPosition()
{
    QRect trayGeometry = m_trayIcon->geometry();
    int w = geometry().width();
    int h = geometry().height();

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
    if (event->key() == Qt::Key_Escape)
        hide();
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
    if (event->type() == QEvent::Type::Paint)
        _setPosition();
    else if (event->type() == QEvent::Wheel)
        if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
            const int STEP = 50;
            QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);
            auto hScrollBar = ui->textEdit->horizontalScrollBar();
            int value = hScrollBar->value()
                    + (wheelEvent->delta() < 0 ? STEP : -STEP);
            hScrollBar->setValue(value);
        }
    return QWidget::event(event);
}
