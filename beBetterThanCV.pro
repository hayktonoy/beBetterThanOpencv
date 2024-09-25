TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/3rdParty/opencv/include

CONFIG(debug, debug | release) {
    LIBS += -L$$PWD/3rdParty/opencv/x64/vc17/lib -lopencv_world4100d
}

CONFIG(release, debug | release) {
    LIBS += -L$$PWD/3rdParty/opencv/x64/vc17/lib -lopencv_world4100
}

SOURCES += \
        main.cpp
