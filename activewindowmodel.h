#ifndef ACTIVEWINDOWMODEL_H
#define ACTIVEWINDOWMODEL_H

#include <QObject>
#include <QStringListModel>

class ActiveWindowModel : public QStringListModel {
  Q_OBJECT
 public:
  ActiveWindowModel(QObject* parent = nullptr);

  [[nodiscard]] QVariant data(const QModelIndex& index,
                              int role = Qt::DisplayRole) const final {
    if (index.row() == rowCount() - 1 && index.column() == 0) {
      switch (role) {
        case Qt::ToolTipRole:
          return {tr("Show all the memos")};
      }
    }
    return QStringListModel::data(index, role);
  }

 signals:
  void titlesUpdated(const QStringList& titles);

 public slots:
  void updateTitles();
};

#endif  // ACTIVEWINDOWMODEL_H
