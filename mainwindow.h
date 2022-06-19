#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "memo.h"
#include "singleton.h"

class ActiveWindowModel;
class FilteredMemosModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public Singleton<MainWindow> {
  Q_OBJECT

 public:
  ~MainWindow();

 protected:
  friend class Singleton<MainWindow>;
  MainWindow(QWidget *parent = nullptr);

 public slots:
  void flushMemos();
  void onMemoIndexChanged(int row);
  void onMemoChanged();
  void onMemoContentChanged();
  void saveContent();
  void toggleForegroundMonitorTimer(bool enabled);
  void saveWindowGeometryToDatabase();
  void loadWindowGeometryFromDatabase();

  void setRunOnSystemStartup(bool enable);

 signals:
  void memoEditableChanged(bool editable);
  void loadSettingsFromDatabase();

 private slots:
  void removeCurrentMemo();

  void toggleSettingsVisibility(bool visible);
  void updateSettingsVisibility();

 private:
  Ui::MainWindow *ui;
  ActiveWindowModel *m_active_window_model;
  FilteredMemosModel *m_filtered_memos_model;
  Memo *m_current_memo;
  int m_current_row;
  bool m_settings_visible;

  QTimer *m_flush_timer;
};
#endif  // MAINWINDOW_H
