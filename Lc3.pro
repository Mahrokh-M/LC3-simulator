QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AssemblerLogic.cpp \
    FileReadWrite.cpp \
    Logic.cpp \
    assembler.cpp \
    lc3instructions.cpp \
    lc3memory.cpp \
    lc3registers.cpp \
    mainWindow.cpp

HEADERS += \
    AssemblerLogic.h \
    FileReadWrite.h \
    Logic.h \
    assembler.h \
    lc3instructions.h \
    lc3memory.h \
    lc3registers.h

FORMS += \
    Logic.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pictures.qrc
