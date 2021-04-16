QT += gui widgets

CONFIG += c++11 console

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mediaplayer.cpp

HEADERS += \
    mainwindow.h \
    mediaplayer.h

INCLUDEPATH += ../../opencv/build/install/include/ \
               ../../../../../Perso/dev/utils/QtGStreamer/install/include/Qt5GStreamer \
               ../../../../../boost/boost_1_75_0/boost_1_75_0

#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_core320.dll  \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_highgui320.dll  \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_imgcodecs320.dll  \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_imgproc320.dll  \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_features2d320.dll  \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_calib3d320.dll \
#LIBS += ../../opencv/build/install/x86/mingw/bin/libopencv_videoio320.dll \


LIBS += ../../../../../Perso/dev/utils/QtGStreamer/install/bin/libQt5GLib-2.0.dll  \
        ../../../../../Perso/dev/utils/QtGStreamer/install/bin/libQt5GStreamer-1.0.dll  \
        ../../../../../Perso/dev/utils/QtGStreamer/install/bin/libQt5GStreamerUi-1.0.dll  \
        ../../../../../Perso/dev/utils/QtGStreamer/install/bin/libQt5GStreamerUtils-1.0.dll
