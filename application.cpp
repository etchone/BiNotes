#include "application.h"

#include <QIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTranslator>

#include "actions.h"
#include "database.h"
#include "mainwindow.h"
#include "systemtrayicon.h"

Application::Application(int argc, char *argv[])
    : QApplication(argc, argv), m_server_name(), m_local_server(nullptr) {}

int Application::exec() {
  if (tryActivateCreatedInstance()) {
    return 1;
  }
  initLocalServer();

  setWindowIcon(QIcon(":/icon"));

  Database::Instance();
  Actions::Instance();

  MainWindow::Instance()->show();

  connect(this, &Application::instanceActivated, Actions::getInstance(),
          &Actions::showWindowTriggered);

  if (SystemTrayIcon::isSystemTrayAvailable()) {
    SystemTrayIcon::Instance();
  }

  int ret = QApplication::exec();
  deinitLocalServer();
  return ret;
}

bool Application::tryActivateCreatedInstance() {
  auto *socket = new QLocalSocket(this);
  m_server_name = QCoreApplication::applicationName();
  socket->connectToServer(m_server_name);
  bool ret = socket->waitForConnected(200);

  if (ret) {
    socket->close();
    socket->deleteLater();
    return true;
  }
  return false;
}

void Application::initLocalServer() {
  assert(m_local_server == nullptr);
  m_local_server = new QLocalServer(this);
  m_local_server->listen(QCoreApplication::applicationName());
  connect(m_local_server, &QLocalServer::newConnection, this,
          &Application::onNewInstanceConnect);
}

void Application::deinitLocalServer() {
  assert(m_local_server != nullptr && m_local_server->isListening());
  m_local_server->close();

  // This server will not be deleted here
  m_local_server->deleteLater();
}

void Application::onNewInstanceConnect() {
  QLocalSocket *socket = m_local_server->nextPendingConnection();
  emit instanceActivated();
  socket->close();
  socket->deleteLater();
}
