#ifndef QKNOTES_H
#define QKNOTES_H

#include <QWidget>
#include "contentmanager.h"
#include "noteblock.h"

class QkNotes : public QWidget
{
    Q_OBJECT

public:
    explicit QkNotes(QWidget *parent = nullptr);
    ~QkNotes() override;

public slots:
    void backup();

private slots:
    void onCommitDataRequest() { m_mgr.save(); }
    void placeholderTextChanged();
    void onNoteBlockNoteDeleted(NoteBlock* noteBlock);
    void onNoteBlockTryMove(NoteBlock* noteBlock);
    void onNoteBlockDragProgress(bool isVertical, qreal progress, NoteBlock* noteBlock);
    void onNoteBlockDragReset();

private:
    ContentManager m_mgr;
    std::vector<NoteBlock*> m_noteBlocks;
    NoteBlockPlaceholder* m_placeholder;

    void _focusToNoteBlock(QPlainTextEdit* noteBlock);
    NoteBlock* _addNoteBlock(NoteBlockContent* content);
    void _setBgColor(QColor color);
    NoteBlock* _findOverlappingNoteBlock(NoteBlock* query);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // QKNOTES_H
