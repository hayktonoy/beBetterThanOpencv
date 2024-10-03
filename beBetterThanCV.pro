TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

# Define the path to OpenCV
OPENCV_PATH = $$PWD/3rdParty/opencv_normal_taza

# Uncomment this line if you need to include the opencv.pri file
# include($$PWD/3rdParty/opencv/opencv.pri)

# Include OpenCV headers
INCLUDEPATH += $$OPENCV_PATH/include
INCLUDEPATH += $$PWD/3rdParty/nlohman

# Link libraries based on the configuration (debug or release)
CONFIG(debug, debug|release) {
    LIBS += -L$$OPENCV_PATH/debug/lib -lopencv_world4100d
    LIBS += -L$$OPENCV_PATH/debug/bin -lopencv_world4100d
} else: CONFIG(release, debug|release) {
    LIBS += -L$$OPENCV_PATH/bin -lopencv_world4100
    LIBS += -L$$OPENCV_PATH/lib -lopencv_world4100
}






SOURCES += \
        arucodetector.cpp \
        main.cpp \
        testertoliinch.cpp \
        utils.cpp \
        warppingtwoimageswithfeaturematcher.cpp

HEADERS += \
    arucodetector.h \
    testertoliinch.h \
    utils.h \
    warppingtwoimageswithfeaturematcher.h
