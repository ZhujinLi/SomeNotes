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
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    setPlainText(content->getText());
}

NoteBlock::~NoteBlock()
{
    delete ui;
}

NoteBlock::DragResult NoteBlock::_endDragging()
{
    if (m_dragState != DragState_dragging)
        return DragResult_none;
    m_dragState = DragState_none;

    if (m_dragDir == DragDir::DragDir_horizontal) {
        int dx = geometry().x() - m_dragStartGeoPos.x();
        if (dx < -geometry().width() * DRAG_THRESHOLD) { // delete
            emit noteDeleted(this);
            return DragResult_deleted;
        }
    } else {
        emit trySwap(this);
    }

    updateGeometry();
    return DragResult_unknown;
}

qreal NoteBlock::_heightOfRows(qreal rows) const
{
    return rows * fontMetrics().height() +
            (document()->documentMargin() + frameWidth()) * 2 +
            contentsMargins().top() + contentsMargins().bottom();
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

    // currently only left dragging is supported
    deltaPos.setX(qMin(deltaPos.x(), 0));

    bool dragPrecond = (event->buttons() & Qt::LeftButton)
        && IS_AUX_KEY_DOWN(DRAG_MOD_KEY);

    bool dragValid = (m_dragState == DragState_dragging)
            || deltaPos.manhattanLength() > QApplication::startDragDistance();

    if (dragPrecond && dragValid) {
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

        if (m_dragDir == DragDir_horizontal)
            emit dragProgress(false, deltaPos.x() / static_cast<qreal>(geometry().width()), this);
        else {
            emit dragProgress(true, deltaPos.y() / static_cast<qreal>(geometry().height()), this);
        }

        m_dragState = DragState_dragging;
    } else if (!dragPrecond) {
        QPlainTextEdit::mouseMoveEvent(event);
    }
}

void NoteBlock::on_NoteBlock_textChanged()
{
    m_content->setText(toPlainText());
    updateGeometry();
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
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
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


QSize NoteBlock::sizeHint() const
{
    int h = static_cast<int>(_heightOfRows(document()->size().height()));
    return {width(), h};
}

QSize NoteBlock::minimumSizeHint() const
{
    return sizeHint();
}
