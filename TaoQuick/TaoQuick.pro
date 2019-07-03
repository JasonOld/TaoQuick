TEMPLATE = lib

TARGET = $$qtLibraryTarget(TaoQuick)

QT += qml quick
CONFIG += plugin c++11 qtquickcompiler
uri = TaoQuick
CONFIG(debug, debug|release){
  CONFIG -=app_bundle
  BundlePath=
  DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../bin/debug/$${BundlePath}$${uri})
} else {
  BundlePath=TaoQuickDemo.app/Contents/MacOS/
  DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../bin/release/$${BundlePath}$${uri})
}

include(../Common/TaoVersion.pri)
include(Qml/TaoQuickDesigner.pri)
HEADERS += \
        Src/taoquick_plugin.h

SOURCES += \
    Src/taoquick_plugin.cpp

RESOURCES += \
    Image.qrc \
    Qml.qrc



# Additional import path used to resolve QML modules in Qt Creator's code model
QML2_IMPORT_PATH += $$_PRO_FILE_PWD_/Qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH += $$_PRO_FILE_PWD_/Qml

!equals(_PRO_FILE_PWD_, $$DESTDIR) {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    win32 {
        copy_qmldir.target ~= s,/,\\\\,g
        copy_qmldir.depends ~= s,/,\\\\,g
    }
    copy_qmldir.commands = $${QMAKE_COPY_FILE} $${copy_qmldir.depends} $${copy_qmldir.target}
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

DISTFILES = qmldir
qmldir.files = qmldir

installPath = $$[QT_INSTALL_QML]/$${uri}
win32 {
    installPath ~= s,/,\\,g
}
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir

