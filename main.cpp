#include "qknotes.h"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow window;
    QkNotes w(&window);
    w.show();

    return a.exec();
}
