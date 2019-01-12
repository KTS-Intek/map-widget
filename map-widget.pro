INCLUDEPATH  += $$PWD\
                $$PWD/../../defines/defines
                $$PWD/../../defines/define-types

include(../qml-geo/qml-geo.pri)


FORMS += \
    $$PWD/map-pgs/mapwidget.ui

HEADERS += \
    $$PWD/map-pgs/mapwidget.h

SOURCES += \
    $$PWD/map-pgs/mapwidget.cpp

