
#ifndef NOTEBLOCK_H
#define NOTEBLOCK_H

#include "noteblockcontent.h"
#include <QPlainTextEdit>

namespace Ui {
class NoteBlock;
}

class NoteBlock : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit NoteBlock(NoteBlockContent* content, QWidget *parent = nullptr);
    ~NoteBlock() override;

    NoteBlockContent* getContent() { return m_content; }

    void enableTranslucent(bool enable);

signals:
    void noteDeleted(NoteBlock* o);
    void trySwap(NoteBlock* o);
    void dragProgress(bool isVertical, qreal progress, NoteBlock* o);
    void dragReset();

private:
    Ui::NoteBlock *ui;
    QPoint m_dragStartMousePos;
    QPoint m_dragStartGeoPos;
    NoteBlockContent* const m_content;

    enum DragDir
    {
        DragDir_unknown,
        DragDir_vertical,
        DragDir_horizontal,
    };
    DragDir m_dragDir;

    enum DragState
    {
        DragState_none,
        DragState_dragging
    };
    DragState m_dragState;

    enum DragResult
    {
        DragResult_none,
        DragResult_unknown,
        DragResult_deleted
    };

    DragResult _endDragging();

    qreal _heightOfRows(qreal rows) const;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void on_NoteBlock_textChanged();

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

    // QWidget interface
public:
    QSize sizeHint() const override;

    // QWidget interface
public:
    QSize minimumSizeHint() const override;
};

class NoteBlockPlaceholder : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit NoteBlockPlaceholder(QWidget *parent);
    ~NoteBlockPlaceholder() override;

private:
    Ui::NoteBlock *ui;
};

#endif // NOTEBLOCK_H
