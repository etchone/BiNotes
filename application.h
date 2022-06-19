#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

#include "qapplication.h"

class QLocalServer;
class Database;
class Actions;
class MainWindow;
class SystemTrayIcon;

class Application : public QApplication {
  Q_OBJECT
 public:
  Application(int argc, char* argv[]);
  ~Application() = default;

  int exec();

  static Application* instance() {
    return static_cast<Application*>(QCoreApplication::instance());
  }

  [[nodiscard]] Database* database() { return m_db; }
  [[nodiscard]] const Database* database() const { return m_db; }
  [[nodiscard]] Actions* actions() { return m_actions; }
  [[nodiscard]] const Actions* actions() const { return m_actions; }
  [[nodiscard]] MainWindow* mainWindow() { return m_mw; }
  [[nodiscard]] const MainWindow* mainWindow() const { return m_mw; }
  [[nodiscard]] SystemTrayIcon* systemTrayIcon() { return m_tray; }
  [[nodiscard]] const SystemTrayIcon* systemTrayIcon() const { return m_tray; }

 private:
  void initLocalServer();
  void deinitLocalServer();
  bool tryActivateCreatedInstance();

 signals:
  void instanceActivated();

 public slots:
  void onNewInstanceConnect();

 private:
  QString m_server_name;
  QLocalServer* m_local_server;

  Database* m_db;
  Actions* m_actions;
  MainWindow* m_mw;
  SystemTrayIcon* m_tray;
};

#endif  // APPLICATION_H
