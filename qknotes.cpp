#include "qknotes.h"
#include "ui_qknotes.h"

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkNotes)
{
    ui->setupUi(this);
}

QkNotes::~QkNotes()
{
    delete ui;
}
