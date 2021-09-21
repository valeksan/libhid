QT -= gui

TEMPLATE = lib
CONFIG += staticlib
TARGET = hid

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += LIB_DEBUG

SOURCES += \
    imp/md5.cpp \
    imp/util.cpp \
    libhid.cpp

HEADERS += \
    imp/md5.hpp \
    imp/util.hpp \
    libhid.h

win32:{
    SOURCES += imp/windows_manager.cpp
    HEADERS += imp/windows_manager.hpp
    DEFINES += _WIN32_WINNT=0x0600
    DEFINES += _WIN32_LEAN_AND_MEAN
    DEFINES += OS_WINDOWS
}
linux:{
    SOURCES += imp/linux_manager.cpp
    HEADERS += imp/linux_manager.hpp
    DEFINES += OS_LINUX
}
macx:{
    SOURCES += imp/mac_manager.cpp
    HEADERS += imp/mac_manager.hpp
    DEFINES += OS_MAC
}

CONFIG(release, debug|release): {
    DESTDIR="$$PWD/build/release"
} else {
    DESTDIR="$$PWD/build/debug"
}

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
