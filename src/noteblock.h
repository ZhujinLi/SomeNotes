#pragma once

#include <QPlainTextEdit>

namespace Ui {
class NoteBlock;
}

class NoteBlockBase : public QPlainTextEdit {
    Q_OBJECT

protected:
    explicit NoteBlockBase(QWidget *parent);
    virtual ~NoteBlockBase() override;

    Ui::NoteBlock *_ui() { return m_ui; }

private:
    Ui::NoteBlock *m_ui;
};

class NoteBlock : public NoteBlockBase {
    Q_OBJECT

public:
    explicit NoteBlock(QSharedPointer<QString> content, QWidget *parent);
    virtual ~NoteBlock() override;

    QSharedPointer<QString> getContent() { return m_content; }

    void enableTranslucent(bool enable);
    void enableHighlight(bool enable);

signals:
    void noteTrashed(NoteBlock *o);
    void trySwap(NoteBlock *o);
    void dragProgress(bool isVertical, qreal progress, NoteBlock *o);
    void dragReset();

private:
    QPoint m_dragStartMousePos;
    QPoint m_dragStartGeoPos;
    QSharedPointer<QString> m_content;

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

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
    virtual void resizeEvent(QResizeEvent *event) override;

private slots:
    void _onTextChanged();
};

class NoteBlockPlaceholder : public NoteBlockBase {
    Q_OBJECT

public:
    explicit NoteBlockPlaceholder(QWidget *parent);
    virtual ~NoteBlockPlaceholder() override;
};
