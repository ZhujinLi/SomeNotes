#include "noteblock.h"
#include "ui_noteblock.h"
#include <QScrollBar>

NoteBlock::NoteBlock(NoteBlockContent* content, QWidget *parent) :
    QPlainTextEdit (parent),
    ui(new Ui::NoteBlock),
    m_content(content),
    m_dragState(DragState_none)
{
    ui->setupUi(this);

    setPlainText(content->getText());

    setMouseTracking(true);
}

NoteBlock::~NoteBlock()
{
    saveContent();
    delete ui;
}


void NoteBlock::saveContent()
{
    m_content->setText(toPlainText());
    m_content->saveIfNeeded();
}

NoteBlock::DragResult NoteBlock::_endDragging()
{
    if (m_dragState != DragState_dragging)
        return DragResult_none;
    m_dragState = DragState_none;

    if (m_dragDir == DragDir::DragDir_horizontal) {
        int dx = geometry().x() - m_dragStartGeoPos.x();
        if (dx < -geometry().width() / 2) { // delete
            emit noteDeleted(this);
            return DragResult_deleted;
        }
    }

    updateGeometry();
    return DragResult_restored;
}

void NoteBlock::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
            && IS_AUX_KEY_DOWN(DRAG_MOD_KEY)) {
        m_dragStartMousePos = QCursor::pos();
        m_dragStartGeoPos = geometry().topLeft();
        m_dragDir = DragDir::DragDir_unknown;
    }
    else {
        QPlainTextEdit::mousePressEvent(event);
    }
}


void NoteBlock::mouseMoveEvent(QMouseEvent *event)
{
    QPoint deltaPos = QCursor::pos() - m_dragStartMousePos;

    bool drag = (event->buttons() & Qt::LeftButton)
        && IS_AUX_KEY_DOWN(DRAG_MOD_KEY);

    drag &= (m_dragState == DragState_dragging)
            || deltaPos.manhattanLength() > QApplication::startDragDistance();

    if (drag) {
        if (m_dragDir == DragDir::DragDir_unknown) {
            m_dragDir = abs(deltaPos.x()) > abs(deltaPos.y()) ? DragDir::DragDir_horizontal : DragDir::DragDir_vertical;
            raise();
        }

        if (m_dragDir == DragDir::DragDir_vertical)
            deltaPos.setX(0);
        else
            deltaPos.setY(0);
        setGeometry(m_dragStartGeoPos.x() + deltaPos.x(), m_dragStartGeoPos.y() + deltaPos.y(),
                    geometry().width(), geometry().height());

        m_dragState = DragState_dragging;
    } else {
        QPlainTextEdit::mouseMoveEvent(event);
    }
}

void NoteBlock::on_NoteBlock_textChanged()
{
    m_changeCount++;
    if (m_changeCount >= 50) {
        saveContent();
        m_changeCount = 0;
    }
}


void NoteBlock::wheelEvent(QWheelEvent *event)
{
    if (IS_AUX_KEY_DOWN(SCROOL_MOD_KEY)) {
        const int STEP = 50;
        QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);
        auto hScrollBar = horizontalScrollBar();
        int value = hScrollBar->value()
                + (wheelEvent->delta() < 0 ? STEP : -STEP);
        hScrollBar->setValue(value);
    }
    return QPlainTextEdit::wheelEvent(event);
}


void NoteBlock::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case DRAG_KEY:
        QApplication::restoreOverrideCursor();
        _endDragging();
        break;
    }
    QPlainTextEdit::keyReleaseEvent(event);
}


NoteBlockPlaceholder::NoteBlockPlaceholder(QWidget *parent) : QPlainTextEdit (parent), ui(new Ui::NoteBlock)
{
    ui->setupUi(this);
}

NoteBlockPlaceholder::~NoteBlockPlaceholder()
{
    delete ui;
}


void NoteBlock::mouseReleaseEvent(QMouseEvent *event)
{
    if (_endDragging() == DragResult_deleted)
        return;
    QPlainTextEdit::mouseReleaseEvent(event);
}
