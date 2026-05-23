QT       += core gui widgets
CONFIG   += c++17
TARGET    = PetGame
TEMPLATE  = app

SOURCES += \
    main.cpp \
    pet.cpp \
    item.cpp \
    job.cpp \
    petmanager.cpp \
    mainwindow.cpp \
    shopdialog.cpp \
    jobdialog.cpp

HEADERS += \
    pet.h \
    item.h \
    job.h \
    petmanager.h \
    mainwindow.h \
    shopdialog.h \
    jobdialog.h

RESOURCES += resources.qrc
