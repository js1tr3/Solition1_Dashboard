TEMPLATE = app
TARGET = dashboard
INCLUDEPATH += .
QT += quick network core

SOURCES += \
    main.cpp \
    solitonudp.cpp

RESOURCES += \
    dashboard.qrc

OTHER_FILES += \
    qml/dashboard.qml \
    qml/DashboardGaugeStyle.qml \
    qml/IconGaugeStyle.qml \
    qml/TachometerStyle.qml \
    qml/TurnIndicator.qml \
    qml/ValueSource.qml

HEADERS += \
    solitonudp.h

DISTFILES += \
#   qml/Dashboard_linear_Gauge_Stype.qml
