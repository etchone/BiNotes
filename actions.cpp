#include "actions.h"
#include <QAction>
#include <QApplication>
#include <QTimer>
#include "mainwindow.h"

Actions::Actions()
    : m_window_hidden(false)
    , m_settings_hidden(false)
    , m_monitor_foreground(false)
    , m_autorun(false) {
    connect(this, &Actions::quitProgramTriggered,
            QApplication::instance(), &QApplication::quit);
}

Actions::~Actions() {
}

QAction* Actions::createDefaultAction(QString text, void (Actions::*slot)()) {
    QAction *action = new QAction(text, this);
    connect(action, &QAction::triggered, this, slot);
    return action;
}

QAction* Actions::createControllableAction(QString text, bool* enabled, void (Actions::*slot)()) {
    QAction *action = new QAction(text, this);
    connect(action, &QAction::triggered, [enabled, this, slot]() {
        if (*enabled) (this->*slot)();
    });
    return action;
}

QAction* Actions::createCheckableAction(QString text, bool* stored_value, void (Actions::*slot)(bool)) {
    QAction *action = new QAction(text, this);
    action->setCheckable(true);
    action->setChecked(*stored_value);
    connect(action, &QAction::toggled, [stored_value, this, slot](bool checked) {
        *stored_value = checked;
        (this->*slot)(checked);
    });
    return action;
}

QAction* Actions::createDefaultActionWithShortcut(QString text, const QKeySequence& shortcut, void (Actions::*slot)()) {
    QAction *action = new QAction(text, this);
    connect(action, &QAction::triggered, this, slot);
    action->setShortcut(shortcut);
    return action;
}

#include "database.h"

void Actions::loadCheckStatesFromDatabase() {
    Database* db = Database::getInstance();
    QString s = db->getSetting("window_hidden");
    QAction *action;
    if (s == "true") {
        action = hideWindowAction();
        m_settings_hidden = true;
        action->trigger();
        action->deleteLater();
    }
    s = db->getSetting("settings_hidden");
    if (s == "true") {
        action = hideSettingsAction();
        action->setChecked(true);
        action->deleteLater();
    }
    s = db->getSetting("monitor");
    if (s == "true") {
        action = monitorForegroundAction();
        action->setChecked(true);
        action->deleteLater();
    }
    s = db->getSetting("autorun");
    if (s == "true") {
        action = autoRunAction();
        action->setChecked(true);
        action->deleteLater();
    }
}

void Actions::saveCheckStatesToDatabase() {
    Database* db = Database::getInstance();

    bool b = MainWindow::getInstance()->isVisible();
    QString str[] = {QString("false"), QString("true")};
    db->setSetting("window_hidden", str[b]);
    b = m_settings_hidden;
    db->setSetting("settings_hidden", str[b]);
    b = m_monitor_foreground;
    db->setSetting("monitor", str[b]);
    b = m_autorun;
    db->setSetting("autorun", str[b]);
}

#include <QMessageBox>

void Actions::verifyRemoveMemo() {
    QMessageBox mbox(QMessageBox::Warning,
                     tr("Confirm"),
                     tr("Are you sure to remove current memo?"),
                     QMessageBox::Yes | QMessageBox::No,
                     MainWindow::getInstance(),
                     Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint |
                     Qt::WindowStaysOnTopHint);
    mbox.setDefaultButton(QMessageBox::No);
    int ret = mbox.exec();

    if (ret == QMessageBox::Yes)
        emit removeMemoTriggered();
}

void Actions::verifyQuitProgram() {
    QMessageBox mbox(QMessageBox::Warning,
                     tr("Confirm"),
                     tr("Are you sure to quit?"),
                     QMessageBox::Yes | QMessageBox::No,
                     MainWindow::getInstance(),
                     Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint |
                     Qt::WindowStaysOnTopHint);
    mbox.setDefaultButton(QMessageBox::No);
    int ret = mbox.exec();

    if (ret == QMessageBox::Yes) {
        emit quitProgramTriggered();
    }
}

void Actions::setRemoveMemoActionEnabled(bool enabled) {
    m_remove_memo_enabled = enabled;
}

QAction* Actions::hideWindowAction() {
    return createDefaultAction(tr("Hide window"), &Actions::hideWindowTriggered);
}
QAction* Actions::showWindowAction() {
    return createDefaultAction(tr("Show window"), &Actions::showWindowTriggered);
}
QAction* Actions::hideSettingsAction() {
    return createCheckableAction(tr("Hide settings"), &m_settings_hidden, &Actions::hideSettingsToggled);
}
QAction* Actions::removeMemoAction() {
    return createDefaultActionWithShortcut(tr("Remove current memo"), QKeySequence(Qt::Key_Delete), &Actions::removeMemoTriggered);
}
QAction* Actions::flushTitlesAction() {
    return createDefaultAction(tr("Flush titles"), &Actions::flushTitlesTriggered);
}
QAction* Actions::monitorForegroundAction() {
    return createCheckableAction(tr("Monitor foreground window"), &m_monitor_foreground, &Actions::monitorForegroundToggled);
}
QAction* Actions::autoRunAction() {
    return createCheckableAction(tr("Run on startup"), &m_autorun, &Actions::autorunToggled);
}
QAction* Actions::quitProgramAction() {
    return createDefaultAction(tr("Exit"), &Actions::quitProgramTriggered);
}
