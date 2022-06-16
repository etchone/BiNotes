#ifndef EDITABLECOMBOBOX_H
#define EDITABLECOMBOBOX_H

#include <QComboBox>
#include <QLineEdit>

class EditableComboBox : public QComboBox {
  Q_OBJECT
 public:
  using QComboBox::QComboBox;
  void mouseDoubleClickEvent(QMouseEvent *) final {
    if (isEditable()) {
      setEditable(false);
    } else {
      setEditable(true);
      if (currentIndex() == count() - 1) lineEdit()->setText("");
      lineEdit()->setPlaceholderText(tr("Memo name (eg. \"note*\")"));
      lineEdit()->setToolTip(
          tr("The memo will show up when it matches the chosen window title"));
      connect(lineEdit(), &QLineEdit::editingFinished, this,
              &EditableComboBox::onLineEditFinishEditing);
    }
  }

 private slots:
  void onLineEditFinishEditing() { setEditable(false); }
};

#endif  // EDITABLECOMBOBOX_H
