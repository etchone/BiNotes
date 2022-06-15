#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "activewindowmodel.h"
#include "filteredmemosmodel.h"
#include <QListView>
#include <QLineEdit>
#include <QAction>
#include <QTimer>
#include "actions.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_active_window_model(nullptr)
    , m_filtered_memos_model(nullptr)
    , m_current_memo(nullptr)
    , m_current_row(0)
    , m_settings_visible(true)
    , m_flush_timer(new QTimer(this)) {
    setWindowTitle(tr("BiNotes"));
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    m_active_window_model = new ActiveWindowModel(ui->combo_title);

    QListView *title_list_view = new QListView(this);
    title_list_view->setModel(m_active_window_model);
    title_list_view->setDragEnabled(true);
    ui->combo_title->setView(title_list_view);
    ui->combo_title->setModel(m_active_window_model);

    m_filtered_memos_model = new FilteredMemosModel(this);

    QListView *name_list_view = new QListView(this);
    name_list_view->setModel(m_filtered_memos_model);
    ui->combo_name->setView(name_list_view);
    ui->combo_name->setModel(m_filtered_memos_model);
    ui->combo_name->setInsertPolicy(QComboBox::InsertAtCurrent);

    connect(ui->handleButton, &WindowHandleButton::middleClicked,
            m_active_window_model, &ActiveWindowModel::updateTitles);

    connect(ui->combo_title, &QComboBox::currentTextChanged,
            this, &MainWindow::flushMemos);

    connect(ui->combo_name, &QComboBox::currentIndexChanged,
            this, &MainWindow::onMemoIndexChanged);

    connect(m_filtered_memos_model, &FilteredMemosModel::rowsInserted,
            this, &MainWindow::onMemoChanged);

    connect(ui->edit_content, &QMarkdownTextEdit::textChanged,
            this, &MainWindow::onMemoContentChanged);

    connect(QApplication::instance(), &QApplication::aboutToQuit,
            this, &MainWindow::saveContent);

    connect(Actions::getInstance(), &Actions::hideWindowTriggered,
            this, &MainWindow::hide);
    connect(Actions::getInstance(), &Actions::showWindowTriggered,
            this, &MainWindow::show);

    connect(Actions::getInstance(), &Actions::hideSettingsToggled,
            [this](bool checked) { toggleSettingsVisibility(!checked); });

    connect(Actions::getInstance(), &Actions::removeMemoTriggered,
            this, &MainWindow::removeCurrentMemo);

    connect(Actions::getInstance(), &Actions::monitorForegroundToggled,
            this, &MainWindow::toggleForegroundMonitorTimer);

    connect(Actions::getInstance(), &Actions::flushTitlesTriggered,
            m_active_window_model, &ActiveWindowModel::updateTitles);

    connect(this, &MainWindow::memoEditableChanged,
            ui->edit_content, &QMarkdownTextEdit::setVisible);
    connect(this, &MainWindow::memoEditableChanged,
            Actions::getInstance(), &Actions::setRemoveMemoActionEnabled);

//    connect(ui->handleButton, &WindowHandleButton::moveFinished,
//            this, &MainWindow::saveWindowGeometryToDatabase);
//    connect(ui->handleButton, &WindowHandleButton::resizeFinished,
//            this, &MainWindow::saveWindowGeometryToDatabase);
    connect(this, &MainWindow::loadSettingsFromDatabase,
            this, &MainWindow::loadWindowGeometryFromDatabase);
    connect(this, &MainWindow::loadSettingsFromDatabase,
            Actions::getInstance(), &Actions::loadCheckStatesFromDatabase);

    connect(Database::getInstance(), &Database::aboutToClose,
            this, &MainWindow::saveWindowGeometryToDatabase);
    connect(Database::getInstance(), &Database::aboutToClose,
            Actions::getInstance(), &Actions::saveCheckStatesToDatabase);

    connect(Actions::getInstance(), &Actions::autorunToggled,
            this, &MainWindow::setRunOnSystemStartup);

    addAction(Actions::getInstance()->removeMemoAction());
    addAction(Actions::getInstance()->quitProgramAction());

    ui->edit_content->setVisible(false);
    Actions::getInstance()->removeMemoAction()->setEnabled(false);

    QTimer::singleShot(0, [this](){
        emit loadSettingsFromDatabase();
        m_active_window_model->updateTitles();
    });
}

MainWindow::~MainWindow() {
    m_flush_timer->disconnect(this, nullptr);
    m_flush_timer->stop();
    delete m_filtered_memos_model;
    delete m_flush_timer;
    delete ui;
}

void MainWindow::flushMemos() {
    qDebug() <<"flushMemos";
    int row = m_current_row;
    bool flushed = m_filtered_memos_model->setFilter(ui->combo_title->currentText());

    if (!flushed)
        return;

    saveContent();

    int count = m_filtered_memos_model->rowCount();
    Q_ASSERT(count != 0);

    if (row >= count) {
        row = count - 1;
    }

    ui->combo_name->setCurrentIndex(count - 1);
    if (row != count - 1)
        ui->combo_name->setCurrentIndex(row);
}

void MainWindow::onMemoIndexChanged(int row) {
    qDebug() <<"onMemoIndexChanged";
    saveContent();
    m_current_row = row;
    onMemoChanged();
}

void MainWindow::onMemoChanged() {
    qDebug() <<"onMemoChanged";
    if (m_current_row == m_filtered_memos_model->rowCount() - 1) {
        m_current_memo = nullptr;
        qDebug() <<m_current_memo;
        emit memoEditableChanged(false);
    } else {
        m_current_memo = &m_filtered_memos_model->getMemo(m_current_row);
        qDebug() <<m_current_memo;
        // ui->combo_name->setEditable(false);

        emit memoEditableChanged(true);

        ui->edit_content->setPlainText(m_filtered_memos_model->data(m_filtered_memos_model->index(m_current_row, 1)).toString());
    }
    updateSettingsVisibility();
}

void MainWindow::onMemoContentChanged() {
    qDebug() <<"onMemoContentChanged";
    assert(m_current_memo != nullptr);
    m_current_memo->setContent(ui->edit_content->toPlainText());
}

void MainWindow::saveContent() {
    qDebug() <<"saveContent";
    if (m_current_memo != nullptr && m_current_memo->isDirty()) {

        // Direct change of memo is not couraged, as its implementation does not
        //   have any code to notify FilteredMemosModel.
        // But since we do not display content at any other place, this way is acceptable
        m_current_memo->setContent(ui->edit_content->toPlainText());
        Database::getInstance()->updateMemo(*m_current_memo);
        qDebug() <<"Memo (" <<m_current_memo->title() << ") saved";
    }
}

void MainWindow::toggleForegroundMonitorTimer(bool enabled) {
    qDebug() <<"toggleForegroundMonitorTimer";
    if (enabled) {
        m_flush_timer->setInterval(1000);
        m_flush_timer->setSingleShot(false);
        connect(m_flush_timer, &QTimer::timeout,
                m_active_window_model, &ActiveWindowModel::updateTitles);
        m_flush_timer->start();
    } else {
        m_flush_timer->disconnect(this, nullptr);
        m_flush_timer->stop();
    }
}

void MainWindow::saveWindowGeometryToDatabase() {
    qDebug() <<"saveWindowGeometryToDatabase";
    QRect rect = geometry();
    Database *db = Database::getInstance();
    QString s = QString("%1 %2 %3 %4").arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
    db->setSetting("geometry", s, true);
}

void MainWindow::loadWindowGeometryFromDatabase() {
    qDebug() <<"loadWindowGeometryFromDatabase";
    QRect rect = geometry();
    Database *db = Database::getInstance();
    QString s = QString("%1 %2 %3 %4").arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
    s = db->getSetting("geometry", s, false);
    QStringList list = s.split(' ');
    if (list.size() != 4)
        return;
    int x, y, w, h;
    bool ok;
    x = list[0].toInt(&ok);
    if (!ok) return;
    y = list[1].toInt(&ok);
    if (!ok) return;
    w = list[2].toInt(&ok);
    if (!ok) return;
    h = list[3].toInt(&ok);
    if (!ok) return;
    setGeometry(QRect(x, y, w, h));
}

#include <QMessageBox>

void MainWindow::removeCurrentMemo() {
    qDebug() <<"removeCurrentMemo";
    if (m_current_memo != nullptr) {
        // remove rows will trigger QComboBox::currentTextChanged
        //  -> MainWindow::onMemoIndexChanged
        //  -> MainWindow::saveContent
        // if m_current_memo is not null, it will try to access a memo that's deleted
        m_current_memo = nullptr;
        qDebug() <<m_current_memo;
        m_filtered_memos_model->removeRows(m_current_row, 1);
    }
}

void MainWindow::toggleSettingsVisibility(bool visible) {
    qDebug() <<"toggleSettingsVisibility";
    m_settings_visible = visible;
    updateSettingsVisibility();
}

void MainWindow::updateSettingsVisibility() {
    qDebug() <<"updateSettingsVisibility";
    bool visible = m_settings_visible || ui->edit_content->isHidden();
    ui->label->setVisible(visible);
    ui->label_2->setVisible(visible);
    ui->combo_name->setVisible(visible);
    ui->combo_title->setVisible(visible);
}

#include <QSettings>

void MainWindow::setRunOnSystemStartup(bool enabled) {
    qDebug() <<"setRunOnSystemStartup";
#ifdef WIN32
    constexpr char auto_run[] = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // code from https://blog.csdn.net/u010058695/article/details/99957960
    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings(auto_run, QSettings::NativeFormat);
    if (enabled) {
        QString application_path = QApplication::applicationFilePath();
        settings->setValue(application_name, application_path.replace("/", "\\"));
    }
    else settings->remove(application_name);
#else
#error Please implement autorun code!
#endif
}
