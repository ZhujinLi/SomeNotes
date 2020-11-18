#include "notesview.h"
#include "common.h"
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QLayout>
#include <QTimer>

NotesView::NotesView(QWidget *parent) : QWidget(parent), m_mgr(CONTENT_PATH(), TRASH_PATH()) {
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    _setBgColor(SOMENOTES_BG_COLOR);

    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);
    layout()->setSpacing(0);

    for (size_t i = 0; i < m_mgr.getContentCount(); i++) {
        QSharedPointer<QString> content = m_mgr.getContent(i);
        _addNoteBlock(content);
    }

    m_placeholder = new NoteBlockPlaceholder(this);
    layout()->addWidget(m_placeholder);
    connect(m_placeholder, &QPlainTextEdit::textChanged, this, &NotesView::_onPlaceholderTextChanged);

    connect(qApp, &QApplication::commitDataRequest, this, &NotesView::_onCommitDataRequest);

    QTimer *autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &NotesView::_onAutoSaveTimeout);
    autoSaveTimer->start(60 * 1000);
}

NotesView::~NotesView() {
    // since mgr is shared
    for (NoteBlock *note : m_noteBlocks)
        delete note;
    delete layout();
}

void NotesView::_focusToNoteBlock(QPlainTextEdit *noteBlock) {
    noteBlock->setFocus();
    QTextCursor cursor = noteBlock->textCursor();
    cursor.movePosition(QTextCursor::End);
    noteBlock->setTextCursor(cursor);
}

NoteBlock *NotesView::_addNoteBlock(QSharedPointer<QString> content) {
    NoteBlock *noteBlock = new NoteBlock(content, this);
    connect(noteBlock, &NoteBlock::noteTrashed, this, &NotesView::_onNoteBlockNoteTrashed);
    connect(noteBlock, &NoteBlock::trySwap, this, &NotesView::_onNoteBlockTryMove);
    connect(noteBlock, &NoteBlock::dragProgress, this, &NotesView::_onNoteBlockDragProgress);
    connect(noteBlock, &NoteBlock::dragReset, this, &NotesView::_onNoteBlockDragReset);
    m_noteBlocks.push_back(noteBlock);
    layout()->addWidget(noteBlock);
    return noteBlock;
}

void NotesView::_setBgColor(QColor color) {
    auto palette = this->palette();
    palette.setColor(QPalette::ColorRole::Background, color);
    setPalette(palette);
}

NoteBlock *NotesView::_findOverlappingNoteBlock(NoteBlock *query) {
    QPoint cursorPos = mapFromGlobal(QCursor::pos());

    for (NoteBlock *tested : m_noteBlocks)
        if (tested != query && tested->geometry().contains(cursorPos))
            return tested;

    return nullptr;
}

void NotesView::_onPlaceholderTextChanged() {
    const QString &text = m_placeholder->toPlainText();
    if (!text.isEmpty()) {
        layout()->removeWidget(m_placeholder);

        QSharedPointer<QString> content = m_mgr.newContent();
        *content = text;

        NoteBlock *noteBlock = _addNoteBlock(content);

        layout()->addWidget(m_placeholder);

        m_placeholder->setPlainText("");

        _focusToNoteBlock(noteBlock);
    }
}

void NotesView::_onNoteBlockNoteTrashed(NoteBlock *noteBlock) {
    for (size_t i = 0; i < m_noteBlocks.size(); i++)
        if (m_noteBlocks[i] == noteBlock) {
            layout()->removeWidget(noteBlock);
            m_noteBlocks.erase(m_noteBlocks.begin() + static_cast<int>(i));
            QSharedPointer<QString> content = noteBlock->getContent();
            delete noteBlock;
            m_mgr.trashContent(content);
            return;
        }
}

void NotesView::_onNoteBlockTryMove(NoteBlock *noteBlock) {
    NoteBlock *tested = _findOverlappingNoteBlock(noteBlock);
    if (tested != nullptr) {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->layout());
        int testedIndex = layout->indexOf(tested);
        layout->removeWidget(noteBlock);
        layout->insertWidget(testedIndex, noteBlock);

        m_mgr.move(noteBlock->getContent(), testedIndex);
    }
}

static qreal _qreal_lerp(qreal a, qreal b, qreal ratio) { return a * (1 - ratio) + b * ratio; }

void NotesView::_onNoteBlockDragProgress(bool isVertical, qreal progress, NoteBlock *noteBlock) {
    if (!isVertical && progress < -DRAG_THRESHOLD) {
        _setBgColor(SOMENOTES_TRASH_COLOR);
        noteBlock->enableHighlight(true);
    } else if (!isVertical && progress < 0) {
        QColor from = SOMENOTES_BG_COLOR;
        QColor to = SOMENOTES_TRASH_COLOR;
        qreal ratio = -progress / DRAG_THRESHOLD * 0.5;
        QColor res =
            QColor::fromRgbF(_qreal_lerp(from.redF(), to.redF(), ratio), _qreal_lerp(from.greenF(), to.greenF(), ratio),
                             _qreal_lerp(from.blueF(), to.blueF(), ratio));
        _setBgColor(res);
        noteBlock->enableHighlight(false);
    } else if (isVertical) {
        NoteBlock *highlightNote = _findOverlappingNoteBlock(noteBlock);
        for (NoteBlock *note : m_noteBlocks)
            if (note == highlightNote)
                note->enableHighlight(true);
            else if (note != noteBlock)
                note->enableHighlight(false);
            else
                note->enableTranslucent(true);

        if (highlightNote)
            _setBgColor(SOMENOTES_BG_COLOR);
        else
            _setBgColor(SOMENOTES_HIGHLIGHT_COLOR);
    } else {
        _setBgColor(SOMENOTES_BG_COLOR);
    }
}

void NotesView::_onNoteBlockDragReset() {
    _setBgColor(SOMENOTES_BG_COLOR);
    for (NoteBlock *note : m_noteBlocks) {
        note->enableTranslucent(false);
        note->enableHighlight(false);
    }
}

void NotesView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case DRAG_KEY:
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        break;
    default:;
    }
    QWidget::keyPressEvent(event);
}
