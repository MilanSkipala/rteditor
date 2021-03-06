HEADERS += \
    application.h \
    includeHeaders.h \
    partsRelated.h \
    mathFunctions.h \
    globalVariables.h \
    database.h \
    sideBarWidget.h \
    workspaceWidget.h \
    itemTypeEnums.h \
    scales.h \
    graphicsScene.h \
    heightPathItem.h \
    bezier.h

SOURCES += \
    main.cpp \
    application.cpp \
    partsRelated.cpp \
    database.cpp \
    sideBarWidget.cpp \
    workspaceWidget.cpp \
    mathFunctions.cpp \
    graphicsScene.cpp \
    heightPathItem.cpp \
    bezier.cpp

QMAKE_CXXFLAGS += -Wextra -Wall -Werror


QT += gui widgets
