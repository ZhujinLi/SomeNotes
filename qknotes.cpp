#include "qknotes.h"
#include "ui_qknotes.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QMenu>
#include <fstream>

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent, Qt::Dialog),
    ui(new Ui::QkNotes)
{
    m_changeCount = 0;

    ui->setupUi(this);
    ui->textEdit->setText(m_mgr.getContent().c_str());

    _initTrayIcon();

    _hideHelpMark();
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
    if (m_changeCount >= 50)
    {
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

void QkNotes::_hideHelpMark()
{
    QIcon icon = windowIcon();

    Qt::WindowFlags flags = windowFlags();

    Qt::WindowFlags helpFlag =
    Qt::WindowContextHelpButtonHint;

    flags = flags & (~helpFlag);

    setWindowFlags(flags);

    setWindowIcon(icon);
}

void QkNotes::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    hide();
    event->ignore();
}

void QkNotes::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        show();
        activateWindow();
        break;
    default:
        ;
    }
}

