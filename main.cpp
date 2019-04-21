#include "qknotes.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QkNotes w;
    w.show();

    return a.exec();
}
