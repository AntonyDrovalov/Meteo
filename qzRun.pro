TEMPLATE = app

QT += widgets charts qml quick

SOURCES += main.cpp \
    zmqlistener.cpp \
    config.cpp

RESOURCES +=

win32:CONFIG(release, debug|release): LIBS += -L"C:/Sources/zeromq-4.1.2/bin/Win32/Release/v120_xp/dynamic/" -l"C:/Sources/zeromq-4.1.2/bin/Win32/Release/v120_xp/dynamic/libzmq"
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Sources/zeromq-4.1.2/bin/Win32/Debug/v120_xp/dynamic/" -l"C:/Sources/zeromq-4.1.2/bin/Win32/Debug/v120_xp/dynamic/libzmq"
else:unix: LIBS += -L/usr/local/lib -lzmq

win32:INCLUDEPATH += "C:/Sources/zeromq-4.1.2/include/"
else:unix:INCLUDEPATH += "/usr/local/include/"

win32:DEPENDPATH += "C:/Sources/zeromq-4.1.2/include/"
else:unix:DEPENDPATH += "/usr/local/include/"

CONFIG += c++11


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    MainForm.ui.qml \
    main.qml \
    err.qml \
    WidgetVisibility.qml \
    WidgetMetar.qml \
    WidgetWind.qml \
    WidgetCurrentTime.qml \
    WidgetVNGO.qml \
    WidgetBar.qml \
    WidgetTemp.qml \
    WidgetCurrentWeather.qml \
    WidgetTrend.qml \
    WidgetAddInfo.qml \
    Widget1.qml \
    WidgetRain.qml \
    Graph.qml \
    WidgetGraph.qml

HEADERS += \
    zmqlistener.h \
    config.h \
    easylogging++.h \
    qmllogging.h

qml_files.path = $$OUT_PWD
qml_files.files = $$PWD/*.qml $$PWD/config.json

INSTALLS+=qml_files

