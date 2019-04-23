#ifndef QKNOTES_H
#define QKNOTES_H

#include <QWidget>
#include "contentmanager.h"

namespace Ui {
class QkNotes;
}

class QkNotes : public QWidget
{
    Q_OBJECT

public:
    explicit QkNotes(QWidget *parent = nullptr);
    ~QkNotes();

private slots:
    void on_textEdit_textChanged();

private:
    Ui::QkNotes *ui;
    ContentManager m_mgr;
    int m_changeCount;

    void _saveContent();

};

#endif // QKNOTES_H
