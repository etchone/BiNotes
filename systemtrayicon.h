#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>

#include "singleton.h"

class SystemTrayIcon : public QSystemTrayIcon,
                       public Singleton<SystemTrayIcon> {
  Q_OBJECT

 private slots:
  void onActivate(ActivationReason reason);

 public:
  explicit SystemTrayIcon(QObject *parent = nullptr);
};

#endif  // SYSTEMTRAYICON_H
