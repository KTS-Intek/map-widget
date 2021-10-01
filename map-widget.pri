# Version=0.0.1
#
# Dependencies
# map-widget
# qml-geo   ->
# guisett-shared
#
#
#


DEFINES += HAS_MAP_WIDGETS


INCLUDEPATH  += $$PWD\
                $$PWD/../../defines/defines
                $$PWD/../../defines/define-types

include(../qml-geo/qml-geo.pri)
include(../../gui/qml-gui-core/qml-gui-core.pri)


FORMS += \
    $$PWD/map-pgs/mapwidget.ui \
    $$PWD/template-pgs/gpsitemspreview.ui

HEADERS += \
    $$PWD/map-pgs/mapwidget.h \
    $$PWD/template-pgs/gpsitemspreview.h

SOURCES += \
    $$PWD/map-pgs/mapwidget.cpp \
    $$PWD/template-pgs/gpsitemspreview.cpp

