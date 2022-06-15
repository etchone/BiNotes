#include "menu.h"
#include "actions.h"
#include <QSystemTrayIcon>

Menu::Menu(MenuType type, QWidget* parent)
    : QMenu(parent)
    , m_menu_type(type)
{
    connect(this, &Menu::aboutToShow,
            this, &Menu::generateItems);
}

#include "mainwindow.h"

void Menu::generateItems() {
    Actions *actions = Actions::getInstance();
    clear();

    if (m_menu_type == ApplicationMenu) {
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            addAction(actions->hideWindowAction());
        }
        addAction(actions->hideSettingsAction());
        addSeparator();
        addAction(actions->removeMemoAction());
        addSeparator();
        addAction(actions->flushTitlesAction());
        addAction(actions->monitorForegroundAction());
        addSeparator();
        addAction(actions->autoRunAction());
        addSeparator();
        addAction(actions->quitProgramAction());
    }

    else if (m_menu_type == SystemTrayMenu) {
        QAction *visibility_action = MainWindow::getInstance()->isVisible() ?
            actions->hideWindowAction() :
            actions->showWindowAction();
        addAction(visibility_action);
        setDefaultAction(visibility_action);
        addAction(actions->hideSettingsAction());
        addSeparator();
        addAction(actions->removeMemoAction());
        addSeparator();
        addAction(actions->flushTitlesAction());
        addAction(actions->monitorForegroundAction());
        addSeparator();
        addAction(actions->autoRunAction());
        addSeparator();
        addAction(actions->quitProgramAction());
    }

    else {
        assert(false);
    }
}
