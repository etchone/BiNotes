#ifndef FILTEREDMEMOSMODEL_H
#define FILTEREDMEMOSMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include <QObject>

#include "database.h"
#include "qglobal.h"

class FilteredMemosModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  explicit FilteredMemosModel(QObject* parent = nullptr)
      : QAbstractTableModel(parent),
        m_filtered_memos({Memo(0, "...", "")}),
        m_filter_expression("dummy") {
    connect(Database::getInstance(), &Database::aboutToClose, this,
            &FilteredMemosModel::clearAndSync);

    setFilter("");
  }

#pragma region Necessary implementation
  [[nodiscard]] QVariant data(const QModelIndex& index,
                              int role = Qt::DisplayRole) const final {
    if (role != Qt::DisplayRole || index.column() > 1 ||
        index.row() >= m_filtered_memos.count()) {
      return {};
    }
    auto& p = m_filtered_memos.at(index.row());
    return index.column() == 0 ? QVariant(p.title()) : QVariant(p.content());
  }

  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const final {
    if (role != Qt::DisplayRole) return {};
    return orientation == Qt::Horizontal
               ? QVariant(section)
               : (section == 0 ? QVariant(tr("title", "databaseTitle"))
                               : QVariant(tr("content", "databaseContent")));
  }

  [[nodiscard]] int rowCount(
      const QModelIndex& parent = QModelIndex()) const final {
    qsizetype&& count = m_filtered_memos.count();
    assert(count <= INT_MAX);
    return parent.isValid() ? 0 : (int)count;
  }

  [[nodiscard]] int columnCount(
      const QModelIndex& parent = QModelIndex()) const final {
    Q_UNUSED(parent);
    return 2;
  }
#pragma endregion

#pragma region Editable items
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const final {
    if (index.column() > 1 || index.row() >= m_filtered_memos.count()) {
      return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  }
  bool setData(const QModelIndex& index, const QVariant& value,
               int role = Qt::EditRole) final {
    if (index.column() > 1 || index.row() >= rowCount() ||
        !value.canConvert<QString>() || role != Qt::EditRole)
      return false;
    QString string = value.toString();

    auto& memo = m_filtered_memos[index.row()];

    if (index.column() == 0)
      memo.setTitle(string);
    else
      memo.setContent(string);

    qDebug() << index.row() << rowCount();

    if (index.row() != rowCount() - 1)
      return Database::getInstance()->updateMemo(memo);
    else {
      memo = Database::getInstance()->createMemo(memo.title());
      beginInsertRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
      m_filtered_memos.emplaceBack(Memo(0, "...", ""));
      endInsertRows();
      return true;
    }
  }
#pragma endregion

#pragma region Insert and remove
  //    bool insertRows(int row, int count, const QModelIndex &parent =
  //    QModelIndex()) final {
  //        Q_UNUSED(parent);
  //        beginInsertRows(QModelIndex(), row, row + count - 1);
  //        while (count--) {
  //            auto memo = MemoDB::Instance()->create(tr("Untitled"));
  //            m_filtered_memos.insert(row, memo);
  //            ++row;
  //        }
  //        endInsertRows();
  //        return true;
  //    }

  bool removeRows(int row, int count,
                  const QModelIndex& parent = QModelIndex()) final {
    if (parent.isValid()) return false;

    if (row < 0) {
      count -= -row;
      row = 0;
    }
    if (count > rowCount() - row) count = rowCount() - row;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int i = row, j = count; j; --j) {
      bool result = Database::getInstance()->eraseMemo(m_filtered_memos[i]);
      if (!result) {
        ++i;
      } else {
        m_filtered_memos.remove(i);
      }
    }

    endRemoveRows();
    return true;
  }
#pragma endregion

  Memo& getMemo(int row) {
    assert(row < rowCount() && row >= 0);
    return m_filtered_memos[row];
  }

 private slots:
  void clearAndSync() {
    if (rowCount() > 1) {
      beginRemoveRows(QModelIndex(), 0, rowCount() - 2);
      for (qsizetype count = m_filtered_memos.size() - 1; count; --count) {
        if (m_filtered_memos[0].isDirty())
          Database::getInstance()->updateMemo(m_filtered_memos[0]);
        m_filtered_memos.pop_front();
      }
      endRemoveRows();
    }
  }

 public slots:

  bool setFilter(const QString& glob_expression) {
    if (glob_expression == m_filter_expression) return false;
    m_filter_expression = glob_expression;
    auto&& list = Database::getInstance()->filterMemo(
        glob_expression == "(All)" ? "" : glob_expression);

    clearAndSync();

    if (list.length()) {
      qsizetype last = list.length() - 1;
      assert(last <= INT_MAX);
      beginInsertRows(QModelIndex(), 0, (int)last);
      m_filtered_memos = list + m_filtered_memos;
      endInsertRows();
    }
    return true;
  }

  void updateMemoTitle(int row, const QString& string) {
    assert(row < rowCount() - 1 && row >= 0);
    auto& memo = m_filtered_memos[row];
    memo.setTitle(string);
    Database::getInstance()->updateMemo(memo);
    emit dataChanged(index(row, 0), index(row, 0));
  }
  void updateMemoContent(int row, const QString& string) {
    assert(row < rowCount() - 1 && row >= 0);
    auto& memo = m_filtered_memos[row];
    memo.setContent(string);
    Database::getInstance()->updateMemo(memo);
    emit dataChanged(index(row, 1), index(row, 1));
  }

  bool deleteMemo(int row) {
    assert(row < rowCount() - 1 && row >= 0);
    auto& memo = m_filtered_memos[row];
    bool result = Database::getInstance()->eraseMemo(memo);

    if (result) {
      beginRemoveRows(QModelIndex(), row, row);
      m_filtered_memos.remove(row);
      endRemoveRows();
    }
    return result;
  }

 private:
  const Memo& addMemo(const QString& title) {
    auto&& memo_r = Database::getInstance()->createMemo(title);
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    auto& memo = *m_filtered_memos.insert(
        (qsizetype)(m_filtered_memos.count() - 1), memo_r);
    endInsertRows();
    return memo;
  }

 private:
  QVector<Memo> m_filtered_memos;
  QString m_filter_expression;
};

#endif  // FILTEREDMEMOSMODEL_H
