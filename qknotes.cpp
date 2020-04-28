#include "pch.h"
#include "qknotes.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QLayout>
#include <QTimer>

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

    connect(qApp, &QApplication::commitDataRequest, this, &QkNotes::onCommitDataRequest);

    QTimer* autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &QkNotes::onAutoSaveTimeout);
    autoSaveTimer->start(60 * 1000);
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
    connect(noteBlock, &NoteBlock::trySwap, this, &QkNotes::onNoteBlockTryMove);
    connect(noteBlock, &NoteBlock::dragProgress, this, &QkNotes::onNoteBlockDragProgress);
    connect(noteBlock, &NoteBlock::dragReset, this, &QkNotes::onNoteBlockDragReset);
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
    QPoint cursorPos = mapFromGlobal(QCursor::pos());

    for (NoteBlock* tested : m_noteBlocks)
        if (tested != query && tested->geometry().contains(cursorPos))
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

void QkNotes::onNoteBlockTryMove(NoteBlock *noteBlock)
{
    NoteBlock* tested = _findOverlappingNoteBlock(noteBlock);
    if (tested != nullptr) {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
        int testedIndex = layout->indexOf(tested);
        layout->removeWidget(noteBlock);
        layout->insertWidget(testedIndex, noteBlock);

        m_mgr.move(noteBlock->getContent(), testedIndex);
    }
}

static qreal _qreal_lerp(qreal a, qreal b, qreal ratio)
{
    return a * (1 - ratio) + b * ratio;
}

void QkNotes::onNoteBlockDragProgress(bool isVertical, qreal progress, NoteBlock* noteBlock)
{
    if (!isVertical && progress < -DRAG_THRESHOLD) {
        _setBgColor(QKNOTES_DEL_COLOR);
        noteBlock->enableHighlight(true);
    }
    else if (!isVertical && progress < 0) {
        QColor from = QKNOTES_BG_COLOR;
        QColor to = QKNOTES_DEL_COLOR;
        qreal ratio = -progress / DRAG_THRESHOLD * 0.5;
        QColor res = QColor::fromRgbF(_qreal_lerp(from.redF(), to.redF(), ratio),
                                      _qreal_lerp(from.greenF(), to.greenF(), ratio),
                                      _qreal_lerp(from.blueF(), to.blueF(), ratio));
        _setBgColor(res);
        noteBlock->enableHighlight(false);
    }
    else if (isVertical) {
        NoteBlock* highlightNote = _findOverlappingNoteBlock(noteBlock);
        for (NoteBlock* note : m_noteBlocks)
            if (note == highlightNote)
                note->enableHighlight(true);
            else if (note != noteBlock)
                note->enableHighlight(false);
            else
                note->enableTranslucent(true);

        if (highlightNote)
            _setBgColor(QKNOTES_BG_COLOR);
        else
            _setBgColor(QKNOTES_HIGHLIGHT_COLOR);
    } else {
        _setBgColor(QKNOTES_BG_COLOR);
    }
}

void QkNotes::onNoteBlockDragReset()
{
    _setBgColor(QKNOTES_BG_COLOR);
    for (NoteBlock* note : m_noteBlocks) {
        note->enableTranslucent(false);
        note->enableHighlight(false);
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
