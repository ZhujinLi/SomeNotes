#include "noteblock.h"
#include "common.h"
#include "controlbar.h"
#include "ui_noteblock.h"
#include <QScrollBar>

NoteBlockBase::NoteBlockBase(QWidget *parent) : QPlainTextEdit(parent), m_ui(new Ui::NoteBlock) {
    m_ui->setupUi(this);
    layout()->setMargin(0);
    layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QFont font = this->font();
    font.setPixelSize(15);
    setFont(font);

    document()->setDocumentMargin(fontMetrics().height() / 3);
}

NoteBlockBase::~NoteBlockBase() { delete m_ui; }

NoteBlock::NoteBlock(QSharedPointer<QString> content, QWidget *parent)
    : NoteBlockBase(parent), m_content(content), m_dragState(DragState_none) {
    enableHighlight(false);
    enableTranslucent(false);
    setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);

    setPlainText(*content);

    connect(this, &QPlainTextEdit::textChanged, this, &NoteBlock::_onTextChanged);

    m_controlBar = new ControlBar(this);
    m_controlBar->setVisible(false);
    layout()->addWidget(m_controlBar);

    connect(m_controlBar, &ControlBar::pressed, this, &NoteBlock::_onControlBarPressed);
    connect(m_controlBar, &ControlBar::moved, this, &NoteBlock::_onControlBarMoved);
    connect(m_controlBar, &ControlBar::released, this, &NoteBlock::_onControlBarReleased);
}

NoteBlock::~NoteBlock() {}

void NoteBlock::enableTranslucent(bool enable) {
    int alpha = enable ? 0x40 : 0xff;

    QPalette palette = this->palette();

    QBrush base = palette.base();
    QColor baseColor = base.color();
    baseColor.setAlpha(alpha);
    base.setColor(baseColor);
    palette.setBrush(QPalette::ColorRole::Base, base);

    QBrush fg = palette.text();
    QColor fgColor = fg.color();
    fgColor.setAlpha(alpha);
    fg.setColor(fgColor);
    palette.setBrush(QPalette::ColorRole::Text, fg);

    setPalette(palette);
}

void NoteBlock::enableHighlight(bool enable) {
    QPalette palette = this->palette();
    QBrush base = palette.base();
    base.setColor(enable ? SOMENOTES_HIGHLIGHT_COLOR : SOMENOTES_BASE_COLOR);
    palette.setBrush(QPalette::ColorRole::Base, base);
    setPalette(palette);
}

void NoteBlock::_onControlBarPressed() {
    m_dragStartMousePos = QCursor::pos();
    m_dragStartGeoPos = geometry().topLeft();
    m_dragDir = DragDir::DragDir_unknown;
}

void NoteBlock::_onControlBarMoved() {
    QPoint deltaPos = QCursor::pos() - m_dragStartMousePos;

    // Currently only left dragging is supported
    deltaPos.setX(qMin(deltaPos.x(), 0));

    bool dragValid =
        (m_dragState == DragState_dragging) || deltaPos.manhattanLength() > QApplication::startDragDistance();

    if (dragValid) {
        if (m_dragDir == DragDir::DragDir_unknown) {
            m_dragDir = abs(deltaPos.x()) > abs(deltaPos.y()) ? DragDir::DragDir_horizontal : DragDir::DragDir_vertical;
            raise();
        }

        if (m_dragDir == DragDir::DragDir_vertical) {
            deltaPos.setX(0);
        } else
            deltaPos.setY(0);
        setGeometry(m_dragStartGeoPos.x() + deltaPos.x(), m_dragStartGeoPos.y() + deltaPos.y(), geometry().width(),
                    geometry().height());

        if (m_dragDir == DragDir_horizontal)
            emit dragProgress(false, deltaPos.x() / static_cast<qreal>(geometry().width()), this);
        else {
            emit dragProgress(true, deltaPos.y() / static_cast<qreal>(geometry().height()), this);
        }

        m_dragState = DragState_dragging;
    }
}

void NoteBlock::_onControlBarReleased() {
    emit dragReset();

    if (m_dragState != DragState_dragging)
        return;
    m_dragState = DragState_none;

    if (m_dragDir == DragDir::DragDir_horizontal) {
        int dx = geometry().x() - m_dragStartGeoPos.x();
        if (dx < -geometry().width() * DRAG_THRESHOLD) { // Trash
            emit noteTrashed(this);
            return;
        }
    } else {
        emit trySwap(this);
    }

    updateGeometry();
}

void NoteBlock::_onTextChanged() {
    *m_content = toPlainText();
    updateGeometry();
}

QSize NoteBlock::sizeHint() const {
    int rows = document()->size().height();
    int h = static_cast<int>(rows * fontMetrics().height() + (document()->documentMargin() + frameWidth()) * 2 +
                             contentsMargins().top() + contentsMargins().bottom());
    return {width(), h};
}

QSize NoteBlock::minimumSizeHint() const { return sizeHint(); }

void NoteBlock::resizeEvent(QResizeEvent *event) {
    updateGeometry();
    QPlainTextEdit::resizeEvent(event);
}

void NoteBlock::focusInEvent(QFocusEvent *event) {
    m_controlBar->setVisible(true);
    NoteBlockBase::focusInEvent(event);
}

void NoteBlock::focusOutEvent(QFocusEvent *event) {
    m_controlBar->setVisible(false);
    NoteBlockBase::focusOutEvent(event);
}

NoteBlockPlaceholder::NoteBlockPlaceholder(QWidget *parent) : NoteBlockBase(parent) {
    setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Expanding);
}

NoteBlockPlaceholder::~NoteBlockPlaceholder() {}
