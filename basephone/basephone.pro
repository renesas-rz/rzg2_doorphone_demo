TEMPLATE = app
TARGET = basephone

QT += quick multimedia

LOCAL_SOURCES = main.cpp 

SOURCES += $$LOCAL_SOURCES
HEADERS += $$LOCAL_HEADERS
RESOURCES += basephone.qrc

SOURCES +=
HEADERS +=


target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/video/basephone
INSTALLS += target


