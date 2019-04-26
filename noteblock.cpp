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
        m_dragStartMousePos = QCursor::pos();
        m_dragStartGeoPos = geometry().topLeft();
    }
    else {
        QPlainTextEdit::mousePressEvent(event);
    }
}


void NoteBlock::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton)
        && QApplication::keyboardModifiers().testFlag(Qt::AltModifier)
        && (event->pos() - m_dragStartMousePos).manhattanLength() > QApplication::startDragDistance()) {
        QPoint deltaPos = QCursor::pos() - m_dragStartMousePos;
        setGeometry(m_dragStartGeoPos.x() + deltaPos.x(), m_dragStartGeoPos.y() + deltaPos.y(),
                    geometry().width(), geometry().height());
    } else {
        QPlainTextEdit::mouseMoveEvent(event);
    }
}
