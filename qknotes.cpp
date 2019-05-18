#include "pch.h"
#include "qknotes.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QLayout>

static const QColor QKNOTES_BG_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);
static const QColor QKNOTES_DEL_COLOR = QColor::fromRgb(0xff, 0x30, 0x30);
static const QColor QKNOTES_SWAP_COLOR = QColor::fromRgb(205, 232, 255);

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    _setBgColor(QKNOTES_BG_COLOR);

    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);
    layout()->setSpacing(0);

    for (size_t i = 0; i < m_mgr.getContentCount(); i++) {
        NoteBlockContent* content = m_mgr.getContent(i);
        _addNoteBlock(content);
    }

    m_placeholder = new NoteBlockPlaceholder(this);
    layout()->addWidget(m_placeholder);
    connect(m_placeholder, &QPlainTextEdit::textChanged, this, &QkNotes::placeholderTextChanged);

    if (m_noteBlocks.size())
        _focusToNoteBlock(m_noteBlocks[0]);
    else
        _focusToNoteBlock(m_placeholder);

    connect(qApp, &QApplication::commitDataRequest, this, &QkNotes::onCommitDataRequest);
}

QkNotes::~QkNotes()
{
    // since mgr is shared
    for (NoteBlock* note : m_noteBlocks)
        delete note;
    delete layout();
}

void QkNotes::_focusToNoteBlock(QPlainTextEdit* noteBlock)
{
    noteBlock->setFocus();
    QTextCursor cursor = noteBlock->textCursor();
    cursor.movePosition(QTextCursor::End);
    noteBlock->setTextCursor(cursor);
}

NoteBlock *QkNotes::_addNoteBlock(NoteBlockContent *content)
{
    NoteBlock* noteBlock = new NoteBlock(content, this);
    connect(noteBlock, &NoteBlock::noteDeleted, this, &QkNotes::onNoteBlockNoteDeleted);
    connect(noteBlock, &NoteBlock::trySwap, this, &QkNotes::onNoteBlockTrySwap);
    connect(noteBlock, &NoteBlock::dragProgress, this, &QkNotes::onNoteBlockDragProgress);
    m_noteBlocks.push_back(noteBlock);
    layout()->addWidget(noteBlock);
    return noteBlock;
}

void QkNotes::_setBgColor(QColor color)
{
    auto palette = this->palette();
    palette.setColor(QPalette::ColorRole::Background, color);
    setPalette(palette);
}

NoteBlock *QkNotes::_findOverlappingNoteBlock(NoteBlock *query)
{
    QRect geo = query->geometry();
    QPoint center = geo.center();

    for (NoteBlock* tested : m_noteBlocks)
        if (tested != query && tested->geometry().contains(center))
            return tested;

    return nullptr;
}

void QkNotes::backup()
{
    m_mgr.backup();
}

void QkNotes::placeholderTextChanged()
{
    const QString& text = m_placeholder->toPlainText();
    if (!text.isEmpty()) {
        layout()->removeWidget(m_placeholder);

        NoteBlockContent* content = m_mgr.newContent();
        content->setText(text);

        NoteBlock* noteBlock = _addNoteBlock(content);

        layout()->addWidget(m_placeholder);

        m_placeholder->setPlainText("");

        _focusToNoteBlock(noteBlock);
    }
}

void QkNotes::onNoteBlockNoteDeleted(NoteBlock *noteBlock)
{
    for (size_t i = 0; i < m_noteBlocks.size(); i++)
        if (m_noteBlocks[i] == noteBlock) {
            layout()->removeWidget(noteBlock);
            m_noteBlocks.erase(m_noteBlocks.begin() + static_cast<int>(i));
            NoteBlockContent* content = noteBlock->getContent();
            delete noteBlock;
            m_mgr.deleteContent(content);
            return;
        }
}

void QkNotes::onNoteBlockTrySwap(NoteBlock *noteBlock)
{
    NoteBlock* tested = _findOverlappingNoteBlock(noteBlock);
    if (tested != nullptr) {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
        QWidget* widgetA = noteBlock;
        QWidget* widgetB = tested;
        int indexA = layout->indexOf(widgetA);
        int indexB = layout->indexOf(widgetB);
        if (indexA > indexB) {
            qSwap(indexA, indexB);
            qSwap(widgetA, widgetB);
        }
        layout->removeWidget(widgetA);
        layout->removeWidget(widgetB);
        layout->insertWidget(indexA, widgetB);
        layout->insertWidget(indexB, widgetA);

        m_mgr.swap(noteBlock->getContent(), tested->getContent());
    }
}

static qreal _qreal_lerp(qreal a, qreal b, qreal ratio)
{
    return a * (1 - ratio) + b * ratio;
}

void QkNotes::onNoteBlockDragProgress(bool isVertical, qreal progress, NoteBlock* noteBlock)
{
    if (!isVertical && progress < -DRAG_THRESHOLD)
        _setBgColor(QKNOTES_DEL_COLOR);
    else if (!isVertical && progress < 0) {
        QColor from = QKNOTES_BG_COLOR;
        QColor to = QKNOTES_DEL_COLOR;
        qreal ratio = -progress / DRAG_THRESHOLD * 0.5;
        QColor res = QColor::fromRgbF(_qreal_lerp(from.redF(), to.redF(), ratio),
                                      _qreal_lerp(from.greenF(), to.greenF(), ratio),
                                      _qreal_lerp(from.blueF(), to.blueF(), ratio));
        _setBgColor(res);
    }
    else if (isVertical && _findOverlappingNoteBlock(noteBlock) != nullptr) {
        _setBgColor(QKNOTES_SWAP_COLOR);
    } else {
        _setBgColor(QKNOTES_BG_COLOR);
    }
}


void QkNotes::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case DRAG_KEY:
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        break;
    default:
        ;
    }
    QWidget::keyPressEvent(event);
}
