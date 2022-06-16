#include "systemtrayicon.h"

#include <QApplication>

#include "mainwindow.h"
#include "menu.h"

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(QApplication::windowIcon(), parent) {
  connect(this, &SystemTrayIcon::activated, this, &SystemTrayIcon::onActivate);
  connect(QApplication::instance(), &QApplication::aboutToQuit, this,
          &SystemTrayIcon::hide);
  connect(QApplication::instance(), &QApplication::aboutToQuit, this,
          &SystemTrayIcon::deleteLater);
  show();

  setContextMenu(new Menu(Menu::SystemTrayMenu, MainWindow::getInstance()));
}

void SystemTrayIcon::onActivate(ActivationReason reason) {
  if (reason == DoubleClick) {
    auto mw = MainWindow::getInstance();
    if (mw->isHidden())
      mw->show();
    else
      mw->hide();
  }
}
