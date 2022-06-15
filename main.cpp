#include "mainwindow.h"

#include <QApplication>
#include "database.h"
#include "actions.h"
#include "menu.h"
#include <QIcon>
#include <QTranslator>
#include "systemtrayicon.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon"));

    Database::Instance();
    Actions::Instance();

    MainWindow::Instance()->show();

    if (SystemTrayIcon::isSystemTrayAvailable()) {
        SystemTrayIcon::Instance();
    }
    return a.exec();
}
