#ifndef WINDOWHANDLEBUTTON_H
#define WINDOWHANDLEBUTTON_H

#include <QObject>
#include <QToolButton>

class WindowHandleButton : public QToolButton {
  Q_OBJECT
 public:
  enum class ActionState { Idle, Move, Resize };

  WindowHandleButton(QWidget *parent = nullptr);

  void mouseMoveEvent(QMouseEvent *e) final;
  void mousePressEvent(QMouseEvent *e) final;
  void mouseReleaseEvent(QMouseEvent *e) final;

 signals:
  void middleClicked();
  void moveFinished(QPoint pos);
  void resizeFinished(QSize size);

 private slots:
  void setAction(ActionState s) {
    m_action = s;
    setText(m_action == ActionState::Idle   ? "T"
            : m_action == ActionState::Move ? "M"
                                            : "R");
  }

 private:
  ActionState m_action;
  QPoint m_last_pos;
};

#endif  // WINDOWHANDLEBUTTON_H
