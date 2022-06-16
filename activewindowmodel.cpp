#include "activewindowmodel.h"

ActiveWindowModel::ActiveWindowModel(QObject* parent)
    : QStringListModel(parent) {}

#if defined(WIN32)
#include <Windows.h>

int ActiveWindowModelEnumWindowProc(HWND hwnd, LPARAM param) {
  auto* list = (QStringList*)param;
  do {
    if (!IsWindowVisible(hwnd)) break;

    int exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST) break;

    char buf[1024];
    int len = GetWindowTextA(hwnd, buf, 1024);

    if (len == 0) break;

    QString name = QString::fromLocal8Bit(buf, len);

    len = GetWindowModuleFileNameA(hwnd, buf, 1024);

    if (len) name.append(" (" + QString::fromLocal8Bit(buf) + ')');

    list->append(std::move(name));
  } while (false);

  return list->size() < 255 ? TRUE : FALSE;
}

void ActiveWindowModel::updateTitles() {
  QStringList titles;
  EnumWindows(ActiveWindowModelEnumWindowProc, (LPARAM)&titles);
  titles.append(tr("(All)"));
  setStringList(titles);

  emit titlesUpdated(titles);
}

#else  // WIN32
#error "Please implement ActiveWindowModel::updateTitles"
#endif  // WIN32

/*
void ActiveWindowModel::updateTitles(const QStringList& titles) {
    if (stringList().isEmpty()) {
        setStringList(titles_uncut);
        return;
    }

    QStringList old_titles = stringList();
    QStringList titles = titles_uncut;
    if (titles.length() > 255)
        titles.remove(256, titles.length() - 255);

    // Calculate Levenstein distance and record operations
    uint8_t distance[256][256];
    enum Operation {
        None,
        Insert,
        Replace,
        Remove
    } operation[256][256];
    operation[0][0] = None;
    distance[0][0] = 0;
    for (int i = 1, _end_i = rowCount(); i < _end_i; ++i) {
        distance[0][i] = i;
        operation[0][i] = Insert;
        distance[i][0] = i;
        operation[i][0] = Remove;
        for (int j = 1, _end_j = titles.length(); j < _end_j; ++j) {
            if (old_titles[i] == titles[j]) {
                distance[i][j] = distance[i - 1][j - 1];
                operation[i][j] = None;
                continue;
            }

            Operation direction;
            uint8_t dis;

            if (distance[i][j - 1] < distance[i - 1][j]) {
                direction = Insert;
                dis = distance[i][j - 1];
            } else {
                direction = Remove;
                dis = distance[i - 1][j];
            }

            if (distance[i - 1][j - 1] <= dis) {
                operation[i][j] = Replace;
                distance[i][j] = distance[i - 1][j - 1] + 1;
            } else {
                operation[i][j] = direction;
                distance[i][j] = dis - 1;
            }
        }
    }

    // Push operations onto stack
    Operation operation_stack[256];
    int i = rowCount() - 1, j = titles.length() - 1, top = 0;
    while (i || j) {
        switch (operation[i][j]) {
        case None:
            operation_stack[top++] = None;
            --i;
            --j;
            break;
        case Insert:
            operation_stack[top++] = Insert;
            --j;
            break;
        case Replace:
            operation_stack[top++] = Replace;
            --i;
            --j;
            break;
        case Remove:
            operation_stack[top++] = Remove;
            --i;
            break;
        }
    }

    // Actually perform operations
    int num_removed = 0;
    while (top) {
        --top;
        switch (operation_stack[top]) {
        case None:
            ++i;
            ++j;
            break;
        case Insert:
            stringList().insert(i - num_removed, titles[j]);
            ++j;
            break;
        case Replace:
            stringList().replace(i - num_removed, titles[j]);
            ++i;
            ++j;
            break;
        case Remove:
            stringList().remove(i - num_removed);
            ++num_removed;
            ++i;
            break;
        }
    }
}
*/
