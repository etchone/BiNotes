#include "database.h"

#include <QApplication>
#include <QDir>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVector>

Database::Database() {
  if (QSqlDatabase::contains("qt_sql_default_connection")) {
    m_db = QSqlDatabase::database("qt_sql_default_connection");
  } else {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!dir.exists()) dir.mkpath(dir.path());
    m_db.setDatabaseName(dir.filePath("data.db"));
  }
  m_db.open();
  Q_ASSERT_X(m_db.isOpen(), "database connection", "database open failure");
  createTablesIfNotExist();

  connect(QApplication::instance(), &QApplication::aboutToQuit, this,
          &Database::aboutToClose);
}

void Database::createTablesIfNotExist() {
  QSqlQuery query(m_db);
  bool result = query.prepare(
      "CREATE TABLE IF NOT EXISTS memo ("
      "  id         INTEGER   PRIMARY KEY     UNIQUE,"
      "  title      CHAR(50)  NOT NULL,"
      "  content    TEXT"
      ");");
  Q_ASSERT(result);
  result = query.exec();
  Q_ASSERT(result);

  result = query.prepare(
      "CREATE TABLE IF NOT EXISTS settings ("
      "  name       CHAR(50)   PRIMARY KEY     NOT NULL    UNIQUE,"
      "  value      CHAR(50)"
      ");");
  Q_ASSERT(result);
  result = query.exec();
  Q_ASSERT(result);
}

QVector<Memo> Database::filterMemo(const QString& title) const {
  Q_ASSERT_X(m_db.isOpen(), "database connection", "database open failure");
  QSqlQuery query(m_db);
  bool query_result;

  query_result = query.prepare("SELECT * FROM memo;");
  Q_ASSERT(query_result);
  query_result = query.exec();
  Q_ASSERT(query_result);

  QVector<Memo> results;
  while (query.next()) {
    auto qid = query.value(0), qtitle = query.value(1),
         qcontent = query.value(2);
    Q_ASSERT(qid.canConvert<int>() && qtitle.canConvert<QString>() &&
             qcontent.canConvert<QString>());
    QString t = qtitle.toString();
    QString expression = '*' + t + '*';

    do {
      qsizetype pos = expression.indexOf("**");
      if (pos == -1) break;
      expression.replace(pos, 2, '*');
    } while (true);

    if (title.isEmpty() ||
        QRegularExpression::fromWildcard(expression).match(title).hasMatch())
      results.emplace_back(qid.toInt(), t, qcontent.toString());
  }

  return results;
}

Memo Database::createMemo(const QString& title) {
  Q_ASSERT_X(m_db.isOpen(), "database connection", "database open failure");
  QSqlQuery query(m_db);
  int id = 1;

  bool query_result = query.prepare("SELECT max(id) from memo;");
  Q_ASSERT(query_result);
  query_result = query.exec();
  Q_ASSERT(query_result);
  if (query.next()) {
    auto qid = query.value(0);
    Q_ASSERT(qid.canConvert<int>());
    id = qid.toInt() + 1;
  }

  query_result = query.prepare("INSERT INTO memo (id, title) VALUES (?, ?);");
  Q_ASSERT(query_result);

  query.bindValue(0, id);
  query.bindValue(1, title);

  query_result = query.exec();
  Q_ASSERT(query_result);
  return {id, title, ""};
}

bool Database::updateMemo(Memo& memo) {
  Q_ASSERT_X(m_db.isOpen(), "database connection", "database open failure");
  QSqlQuery query(m_db);
  bool query_result;

  if (memo.id() == 0) return false;

  switch (memo.m_dirty) {
    case Memo::Fresh:
      return true;
    case Memo::TitleDirty:
      query_result = query.prepare("UPDATE memo SET title = ? WHERE id = ?;");
      Q_ASSERT(query_result);
      query.bindValue(0, memo.title());
      query.bindValue(1, memo.id());
      break;
    case Memo::ContentDirty:
      query_result = query.prepare("UPDATE memo SET content = ? WHERE id = ?;");
      Q_ASSERT(query_result);
      query.bindValue(0, memo.content());
      query.bindValue(1, memo.id());
      break;
    case Memo::BothDirty:
      query_result =
          query.prepare("UPDATE memo SET title = ?, content = ? WHERE id = ?;");
      Q_ASSERT(query_result);
      query.bindValue(0, memo.title());
      query.bindValue(1, memo.content());
      query.bindValue(2, memo.id());
      break;
  }

  query_result = query.exec();
  if (query_result) memo.m_dirty = Memo::Fresh;

  return query_result;
}

bool Database::eraseMemo(Memo& memo) {
  Q_ASSERT_X(m_db.isOpen(), "database connection", "database open failure");

  if (memo.id() == 0) return true;

  QSqlQuery query(m_db);
  bool query_result;

  query_result = query.prepare("DELETE FROM memo WHERE id = ?;");
  Q_ASSERT(query_result);
  query.bindValue(0, memo.id());

  query_result = query.exec();

  if (query_result && query.numRowsAffected()) memo.m_id = 0;

  return query_result;
}

QString Database::getSetting(const QString& name, const QString& default_value,
                             bool create_if_not_found) {
  QSqlQuery query(m_db);
  bool result;

  result = query.prepare("SELECT value FROM settings WHERE name = ?;");
  Q_ASSERT(result);
  query.bindValue(0, name);

  result = query.exec();
  Q_ASSERT(result);

  if (query.next()) {
    Q_ASSERT(query.value(0).canConvert<QString>());
    return query.value(0).toString();
  }

  if (create_if_not_found) {
    result = insertSetting(name, default_value);
    Q_ASSERT(result);
  }
  return default_value;
}

bool Database::setSetting(const QString& name, const QString& value,
                          bool create_if_not_found) {
  if (updateSetting(name, value)) return true;

  if (create_if_not_found) {
    return insertSetting(name, value);
  }
  return false;
}

bool Database::insertSetting(const QString& name, const QString& value) {
  QSqlQuery query(m_db);
  bool result;

  result = query.prepare("INSERT INTO settings VALUES (?, ?); ");
  Q_ASSERT(result);
  query.bindValue(0, name);
  query.bindValue(1, value);

  return query.exec();
}

bool Database::updateSetting(const QString& name, const QString& value) {
  QSqlQuery query(m_db);
  bool result;

  result = query.prepare("UPDATE settings SET value = ? WHERE name = ?;");
  Q_ASSERT(result);
  query.bindValue(0, value);
  query.bindValue(1, name);

  return query.exec() && query.numRowsAffected() != 0;
}
