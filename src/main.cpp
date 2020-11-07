#include "mainwin.h"
#include "pch.h"
#include "somenotes.h"
#include <QApplication>
#include <QDir>
#include <QMainWindow>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);

    g_dataDir = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DataLocation)[0];
    QDir().mkpath(g_dataDir);

    MainWin w;
    w.setWindowIcon(QIcon(":/images/app.png"));

    return a.exec();
}
