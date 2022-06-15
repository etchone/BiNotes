#include "memos.h"
#include <QTextDocument>

Memos::Memos(QObject *parent) : QObject(parent), m_memos()
{

}


MemoVector Memos::filter(const QRegularExpression& regular_expression) {
    MemoVector container;
    for (auto itr = m_memos.begin(), _end = m_memos.end(); itr != _end; ++itr) {
        if (regular_expression.match(itr->name).hasMatch()) {
            container.push_back(*itr);
        }
    }
    return container;
}

const Memo& Memos::createMemo(const QString& title) {
    auto& memo = m_memos.emplaceBack(title, QTextDocument(this));
    return memo;
}

Memos Memos::fromByteArray(const QByteArray& array) {
    QDataStream io(array);
    uint32_t magic;
    io >>magic;

    if (magic != 0x61432732)
        return Memos();

    qsizetype count;
    io>>count;

    Memos memos;

    while (count--) {
        QString title, content;
        {
            qsizetype title_len;
            io >>title_len;
            char *buf = new char[title_len];
            int len_read = io.readRawData(buf, title_len);
            assert(len_read == title_len);
            title = QString::fromUtf8(buf, len_read);
            delete[] buf;
        }
        {
            qsizetype content_len;
            io >>content_len;
            char *buf = new char[content_len];
            int len_read = io.readRawData(buf, content_len);
            assert(len_read == content_len);
            content = QString::fromUtf8(buf, len_read);
            delete[] buf;
        }
        memos.m_memos.emplaceBack(std::move(title), std::move(content));
    }
    return memos;
}

QByteArray Memos::toByteArray(void) const {
    QByteArray arr;
    QDataStream io(&arr, QIODeviceBase::WriteOnly);
    io <<0x61432732 <<m_memos.count();
    for (auto itr = m_memos.begin(), _end = m_memos.end(); itr != _end; ++itr) {
        {
            QByteArray title_bytes = itr->name.toUtf8();
            io <<title_bytes.length() <<title_bytes;
        }
        {
            QByteArray content_bytes = itr->content.toUtf8();
            io <<content_bytes.length() <<content_bytes;
        }
    }
    return arr;
}
