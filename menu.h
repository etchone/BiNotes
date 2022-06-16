#ifndef MENU_H
#define MENU_H

#include <QMenu>

#include "singleton.h"

class Menu : public QMenu {
  Q_OBJECT
 public:
  enum MenuType { ApplicationMenu, SystemTrayMenu };

  Menu(MenuType type, QWidget* parent);

 private slots:
  void generateItems();

 private:
  MenuType m_menu_type;
};

#endif  // MENU_H
