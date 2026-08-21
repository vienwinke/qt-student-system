QT += core gui widgets

CONFIG += c++17
TARGET = StudentSystem
TEMPLATE = app

INCLUDEPATH += /usr/include/mysql

SOURCES += \
    main.cpp \
    Student.cpp \
    Score.cpp \
    Calculator.cpp \
    MainWindow.cpp

HEADERS += \
    Student.h \
    Score.h \
    Calculator.h \
    MainWindow.h

LIBS += -lmysqlclient

# Windows 下如果用 MSVC + MySQL Connector，取消下面注释并按实际路径修改
# INCLUDEPATH += C:/Program Files/MySQL/MySQL Server 8.0/include
# LIBS += -L"C:/Program Files/MySQL/MySQL Server 8.0/lib" -llibmysql
