#include "noteblock.h"
#include "ui_noteblock.h"

NoteBlock::NoteBlock(QWidget *parent) :
    QPlainTextEdit (parent),
    ui(new Ui::NoteBlock)
{
    ui->setupUi(this);
}

NoteBlock::~NoteBlock()
{
    delete ui;
}
