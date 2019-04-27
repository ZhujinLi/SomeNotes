#include "noteblock.h"
#include "ui_noteblock.h"
#include <QScrollBar>

NoteBlock::NoteBlock(ContentManager* mgr, QWidget *parent) :
    QPlainTextEdit (parent),
    ui(new Ui::NoteBlock),
    m_mgr(mgr)
{
    ui->setupUi(this);

    setMouseTracking(true);
}

NoteBlock::~NoteBlock()
{
    saveContent();
    delete ui;
}


void NoteBlock::saveContent()
{
    m_mgr->setContent(toPlainText());
    m_mgr->saveIfNeeded();
}

void NoteBlock::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
            && IS_AUX_KEY_DOWN(DRAG_MOD_KEY)) {
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
        && IS_AUX_KEY_DOWN(DRAG_MOD_KEY)
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


void NoteBlock::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case DRAG_KEY:
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        break;
    default:
        ;
    }
    QPlainTextEdit::keyPressEvent(event);
}

void NoteBlock::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case DRAG_KEY:
        QApplication::restoreOverrideCursor();
        break;
    }
    QPlainTextEdit::keyReleaseEvent(event);
}

