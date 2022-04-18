TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix {
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/9/include
}

QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

SOURCES += \
        CMatrix.cpp \
        main.cpp

HEADERS += \
    CMatrix.h \
    Common.h
