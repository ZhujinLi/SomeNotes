#include "qknotes.h"
#include "ui_qknotes.h"
#include <QDebug>
#include <QDir>
#include <fstream>

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent, Qt::Dialog),
    ui(new Ui::QkNotes)
{
    m_changeCount = 0;

    ui->setupUi(this);
    ui->textEdit->setText(m_mgr.getContent().c_str());

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/images/tray.png"));
    m_trayIcon->show();

    _hideHelpMark();
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
