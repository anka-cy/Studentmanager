QT += core gui widgets

CONFIG += c++17

TARGET = StudentManager
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/models/Student.cpp \
    src/models/UndergraduateStudent.cpp \
    src/models/GraduateStudent.cpp \
    src/models/PhDStudent.cpp \
    src/services/StudentManager.cpp \
    src/persistence/PersistenceManager.cpp \
    src/gui/MainWindow.cpp \
    src/gui/StudentDialog.cpp

HEADERS += \
    src/models/Student.h \
    src/models/UndergraduateStudent.h \
    src/models/GraduateStudent.h \
    src/models/PhDStudent.h \
    src/exceptions/StudentExceptions.h \
    src/services/StudentManager.h \
    src/persistence/PersistenceManager.h \
    src/gui/MainWindow.h \
    src/gui/StudentDialog.h