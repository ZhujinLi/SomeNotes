#pragma once

#include "noteblockcontent.h"
#include <QPlainTextEdit>

namespace Ui {
class NoteBlock;
}

class NoteBlock : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit NoteBlock(NoteBlockContent *content, QWidget *parent);
    virtual ~NoteBlock() override;

    NoteBlockContent *getContent() { return m_content; }

    void enableTranslucent(bool enable);
    void enableHighlight(bool enable);

signals:
    void noteTrashed(NoteBlock *o);
    void trySwap(NoteBlock *o);
    void dragProgress(bool isVertical, qreal progress, NoteBlock *o);
    void dragReset();

public slots:
    void on_NoteBlock_textChanged();

private:
    Ui::NoteBlock *ui;
    QPoint m_dragStartMousePos;
    QPoint m_dragStartGeoPos;
    NoteBlockContent *const m_content;

    enum DragDir {
        DragDir_unknown,
        DragDir_vertical,
        DragDir_horizontal,
    };
    DragDir m_dragDir;

    enum DragState { DragState_none, DragState_dragging };
    DragState m_dragState;

    enum DragResult { DragResult_none, DragResult_unknown, DragResult_trashed };

    DragResult _endDragging();

    qreal _heightOfRows(qreal rows) const;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
};

class NoteBlockPlaceholder : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit NoteBlockPlaceholder(QWidget *parent);
    virtual ~NoteBlockPlaceholder() override;

private:
    Ui::NoteBlock *ui;
};
