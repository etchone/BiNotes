#ifndef MEMOS_H
#define MEMOS_H

#include <QObject>

#include <QRegularExpression>
#include <QTextDocument>

struct Memo {
    QString name;
    QString content;
};

using MemoVector = QVector<Memo>;
class Memos : public QObject
{
public:

    Memos(QObject*parent = nullptr);
    Memos(const Memos& other) : m_memos(other.m_memos) {}
    Memos(Memos&& other) : m_memos(std::move(other.m_memos)) {}

public:
    // glob
    MemoVector filter(QStringView pattern,
                         Qt::CaseSensitivity cs = Qt::CaseInsensitive) {
        return filter(QRegularExpression::fromWildcard(pattern, cs));
    }
    MemoVector filter(const QRegularExpression& regular_expression);
    const Memo& createMemo(const QString& title);

    QByteArray toByteArray(void) const;
    static Memos fromByteArray(const QByteArray& array);

private:
    MemoVector m_memos;
};

#endif // MEMOS_H
