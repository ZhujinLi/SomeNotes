#ifndef QKNOTES_H
#define QKNOTES_H

#include <QWidget>

namespace Ui {
class QkNotes;
}

class QkNotes : public QWidget
{
    Q_OBJECT

public:
    explicit QkNotes(QWidget *parent = nullptr);
    ~QkNotes();

private:
    Ui::QkNotes *ui;
};

#endif // QKNOTES_H
