#ifndef NOTEBLOCK_H
#define NOTEBLOCK_H
#include <QPlainTextEdit>

namespace Ui {
class NoteBlock;
}

class NoteBlock : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit NoteBlock(QWidget *parent = nullptr);
    ~NoteBlock() override;

private:
    Ui::NoteBlock *ui;
    QPoint m_dragStartPos;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // NOTEBLOCK_H
