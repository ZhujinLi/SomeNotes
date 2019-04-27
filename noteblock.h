
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

    void saveContent();

private:
    Ui::NoteBlock *ui;
    QPoint m_dragStartMousePos;
    QPoint m_dragStartGeoPos;
    int m_changeCount = 0;
    NoteBlockContent* const m_content;

    enum DragDir
    {
        unknown,
        vertical,
        horizontal,
    };
    DragDir m_dragDir;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;

    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void on_NoteBlock_textChanged();

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event) override;
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
