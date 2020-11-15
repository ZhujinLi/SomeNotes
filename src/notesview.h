#pragma once

#include "contentmanager.h"
#include "noteblock.h"
#include <QWidget>

class NotesView : public QWidget {
    Q_OBJECT

public:
    explicit NotesView(QWidget *parent);
    virtual ~NotesView() override;

private:
    ContentManager m_mgr;
    std::vector<NoteBlock *> m_noteBlocks;
    NoteBlockPlaceholder *m_placeholder;

    virtual void keyPressEvent(QKeyEvent *event) override;

    void _focusToNoteBlock(QPlainTextEdit *noteBlock);
    NoteBlock *_addNoteBlock(QString *content);
    void _setBgColor(QColor color);
    NoteBlock *_findOverlappingNoteBlock(NoteBlock *query);

private slots:
    void _onCommitDataRequest() { m_mgr.save(); }
    void _onPlaceholderTextChanged();
    void _onNoteBlockNoteTrashed(NoteBlock *noteBlock);
    void _onNoteBlockTryMove(NoteBlock *noteBlock);
    void _onNoteBlockDragProgress(bool isVertical, qreal progress, NoteBlock *noteBlock);
    void _onNoteBlockDragReset();
    void _onAutoSaveTimeout() { m_mgr.save(); }
};
