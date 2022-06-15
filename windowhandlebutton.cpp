#include "windowhandlebutton.h"
#include <QMouseEvent>
#include <QApplication>
#include "menu.h"
#include "mainwindow.h"

WindowHandleButton::WindowHandleButton(QWidget* parent)
    : QToolButton(parent),
      m_action(ActionState::Idle),
      m_last_pos()
{
    setMenu(new Menu(Menu::ApplicationMenu, this));
}

void WindowHandleButton::mousePressEvent(QMouseEvent *e) {
    qDebug() <<"P" <<e->button() <<"+" <<e->buttons();

    // Click left button while right button is pressed to close
//    if (e->button() == Qt::LeftButton && e->buttons().testFlags(Qt::LeftButton | Qt::RightButton)) {
//        QApplication::quit();
//        return;
//    }
    m_last_pos = e->globalPosition().toPoint();
    switch(e->button()) {
    case Qt::LeftButton:
        setAction(ActionState::Move);
        break;
    case Qt::RightButton:
        if (e->buttons().testFlag(Qt::LeftButton))
            setAction(ActionState::Resize);
        else
            showMenu();
        break;

    case Qt::MiddleButton:
        emit middleClicked();
    default:
        break;
    }
}

void WindowHandleButton::mouseReleaseEvent(QMouseEvent *e) {
    qDebug() <<"R" <<e->button() <<"+" <<e->buttons();
    // Releasing right while left is pressed: Resize -> Move
    // Releasing Left: Move/Resize -> Idel
    if (e->button() == Qt::RightButton && e->buttons().testAnyFlag(Qt::LeftButton)) {
        if (m_action == ActionState::Resize)
            emit resizeFinished(MainWindow::getInstance()->size());
        setAction(ActionState::Move);
    }
    else if (e->button() == Qt::LeftButton) {
        if (m_action == ActionState::Resize)
            emit resizeFinished(MainWindow::getInstance()->size());
        else if (m_action == ActionState::Move)
            emit moveFinished(MainWindow::getInstance()->pos());
        setAction(ActionState::Idle);
    }
    if (e->buttons() == Qt::NoButton)
        m_last_pos = QPoint();
}

void WindowHandleButton::mouseMoveEvent(QMouseEvent *e) {
    QToolButton::mouseMoveEvent(e);

    if (m_action == ActionState::Idle)
        return;

    QPoint current_pos = e->globalPosition().toPoint();
    QWidget* win = MainWindow::getInstance();
    QPoint pos = win->pos();

    if (m_action == ActionState::Move) {
        pos += current_pos - m_last_pos;
        win->move(pos);
    } else if (m_action == ActionState::Resize) {

        // Resize based on the left bottom point
        QSize size = win->size();
        pos += {0, current_pos.y() - m_last_pos.y()};
        size += {current_pos.x() - m_last_pos.x(), m_last_pos.y() - current_pos.y()};
        win->resize(size);

        // If the window cannot reach target size, this will stop it from moving
        win->move(pos + QPoint(0, size.height() - win->size().height()));
    }

    m_last_pos = current_pos;
}
