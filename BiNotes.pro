QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    cpplibraries/

SOURCES += \
    actions.cpp \
    activewindowmodel.cpp \
    application.cpp \
    database.cpp \
    main.cpp \
    mainwindow.cpp \
    menu.cpp \
    systemtrayicon.cpp \
    windowhandlebutton.cpp \
    cpplibraries/singleton.cpp

HEADERS += \
    actions.h \
    activewindowmodel.h \
    application.h \
    database.h \
    editablecombobox.h \
    filteredmemosmodel.h \
    mainwindow.h \
    memo.h \
    menu.h \
    singleton.h \
    systemtrayicon.h \
    windowhandlebutton.h \
    cpplibraries/singleton.h \
    cpplibraries/collectable.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:LIBS += -lUser32

include (3rdparty/qmarkdowntextedit/qmarkdowntextedit.pri)

include (3rdparty/sqlite3/sqlite3.pri)

RESOURCES += \
    resources.qrc

TRANSLATIONS = BiNotes_zh.ts

RC_ICONS = notepad-icon.ico
ICON = notepad-icon.ico
