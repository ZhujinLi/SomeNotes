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
        m_dragDir = DragDir::unknown;
    }
    else {
        QPlainTextEdit::mousePressEvent(event);
    }
}


void NoteBlock::mouseMoveEvent(QMouseEvent *event)
{
    QPoint deltaPos = QCursor::pos() - m_dragStartMousePos;
    if ((event->buttons() & Qt::LeftButton)
        && QApplication::keyboardModifiers().testFlag(Qt::AltModifier)
        && deltaPos.manhattanLength() > QApplication::startDragDistance()) {
            if (m_dragDir == DragDir::unknown)
                m_dragDir = abs(deltaPos.x()) > abs(deltaPos.y()) ? DragDir::horizontal : DragDir::vertical;

            if (m_dragDir == DragDir::vertical)
                deltaPos.setX(0);
            else
                deltaPos.setY(0);
        setGeometry(m_dragStartGeoPos.x() + deltaPos.x(), m_dragStartGeoPos.y() + deltaPos.y(),
                    geometry().width(), geometry().height());
    } else {
        QPlainTextEdit::mouseMoveEvent(event);
    }
}
