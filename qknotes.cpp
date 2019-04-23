#include "qknotes.h"
#include "ui_qknotes.h"
#include <QDebug>
#include <QDir>
#include <fstream>

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkNotes)
{
    ui->setupUi(this);
    ui->textEdit->setText(m_mgr.getContent().c_str());

    m_changeCount = 0;
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
