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


void NoteBlock::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
            && QApplication::keyboardModifiers().testFlag(Qt::AltModifier)) {
        m_dragStartPos = event->pos();
    }
    else {
        QPlainTextEdit::mousePressEvent(event);
    }
}


void NoteBlock::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)
        && QApplication::keyboardModifiers().testFlag(Qt::AltModifier)
        && (event->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()) {
        QPoint deltaPos = event->pos() - m_dragStartPos;
        setGeometry(geometry().x() + deltaPos.x(), geometry().y() + deltaPos.y(),
                    geometry().width(), geometry().height());
    } else {
        QPlainTextEdit::mouseMoveEvent(event);
    }
}
