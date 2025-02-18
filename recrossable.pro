QT += quick
CONFIG += c++14
CONFIG += config

#CONFIG += sanitizer sanitize_address

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    crossword.cpp \
    drawablecell.cpp \
    characterrecognizer.cpp \
    qqwing/src/cpp/qqwing.cpp

LIBS += -ldlib
RESOURCES += qml.qrc \
    patterns.qrc \
    wordlists.qrc \
    data.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    crossword.h \
    qqwing/target/automake/config.h \
    drawablecell.h \
    characterrecognizer.h

INCLUDEPATH += \
    qqwing/target/automake

linux-oe-g++ {
    LIBS += -lqsgepaper
    DEFINES += REMARKABLE_DEVICE

    # Someone™ removed the dlib and openblas from the device for some reason, but it's still in the toolchain
    dlibso.files = \
        $$[QT_INSTALL_PREFIX]/lib/libdlib.so.19.8.99
#        $$[QT_INSTALL_PREFIX]/lib/libopenblas.so.0 \
#        $$[QT_INSTALL_PREFIX]/lib/libopenblas_cortexa9p-r0.2.19.so \

    dlibso.path = /usr/lib/
    INSTALLS += dlibso
}
