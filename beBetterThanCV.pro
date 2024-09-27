TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt



win32 {
    INCLUDEPATH += $$PWD/3rdParty/opencv/include
    CONFIG(debug, debug | release) {
        LIBS += -L$$PWD/3rdParty/opencv/x64/vc17/lib -lopencv_world4100d
    }
    CONFIG(release, debug | release) {
        LIBS += -L$$PWD/3rdParty/opencv/x64/vc17/lib -lopencv_world4100
    }
}

unix {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -lopencv_core -lopenh -lopenv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio
}




SOURCES += \
        main.cpp
