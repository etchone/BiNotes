#ifndef MEMODB_H
#define MEMODB_H

#include <memo.h>
#include <singleton.h>
#include <QSqlDatabase>
#include <QObject>

class Database : public QObject, public Singleton<Database>
{
    Q_OBJECT

signals:
    void aboutToClose();

public:
    QVector<Memo> filterMemo(const QString& glob_title) const;

    Memo createMemo(const QString& title);

    bool updateMemo(Memo& memo);
    bool eraseMemo(Memo& memo);

    QString getSetting(const QString& name, const QString& default_value = "", bool create_if_not_found = false);
    bool setSetting(const QString& name, const QString& value, bool create_if_not_found = true);

    ~Database();

protected:
    bool insertSetting(const QString& name, const QString& value);
    bool updateSetting(const QString& name, const QString& value);
    // bool deleteSetting(const QString& name);

    friend class Singleton<Database>;
    Database();

private:
    void createTablesIfNotExist();

private:
    QSqlDatabase m_db;
};

#endif // MEMODB_H
