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
};

#endif // NOTEBLOCK_H
