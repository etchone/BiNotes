#ifndef MEMO_H
#define MEMO_H

#include <QBitArray>
#include <QString>

class Memo {
 public:
  [[nodiscard]] int id() const { return m_id; }
  [[nodiscard]] bool isDirty() const { return m_dirty != Fresh; }
  [[nodiscard]] const QString& title() const { return m_title; }
  [[nodiscard]] const QString& content() const { return m_content; }

  void setTitle(const QString& title) {
    m_title = title;
    m_dirty |= TitleDirty;
  }

  void setContent(const QString& content) {
    m_content = content;
    m_dirty |= ContentDirty;
  }

  Memo(Memo&&) = default;
  Memo(const Memo&) = default;
  Memo() : m_id(0), m_dirty(Fresh) {}
  Memo(int id, const QString& title, const QString& content)
      : m_id(id), m_dirty(Fresh), m_title(title), m_content(content) {}

  Memo& operator=(const Memo&) = default;

 protected:
  friend class Database;

 private:
  int m_id;

  enum DirtyType {
    Fresh = 0x00,
    TitleDirty = 0x01,
    ContentDirty = 0x02,
    BothDirty = 0x03
  };

  uint8_t m_dirty;

  QString m_title;
  QString m_content;
};

#endif  // MEMO_H
