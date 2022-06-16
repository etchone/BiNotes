#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
class QLocalServer;

class Application : public QApplication {
  Q_OBJECT
 public:
  Application(int argc, char* argv[]);
  ~Application() = default;

  int exec();

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
};

#endif  // APPLICATION_H
