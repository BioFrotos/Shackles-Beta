QT += widgets texttospeech printsupport

HEADERS	    =   mainwindow.h \
                diagramitem.h \
                diagramscene.h \
                arrow.h \
                diagramtextitem.h\
                SizeGripItem.h
SOURCES	    =   mainwindow.cpp \
                diagramitem.cpp \
                main.cpp \
                arrow.cpp \
                diagramtextitem.cpp \
                diagramscene.cpp \
                SizeGripItem.cpp
RESOURCES   = \
    diagramscene.qrc

VERSION =1.0.0.5
QMAKE_TARGET_COMPANY= Raven Made
QMAKE_TARGET_PRODUCT= AppDiagram project
QMAKE_TARGET_DESCRIPTION= Diagram redactor
QMAKE_TARGET_COPYRIGHT= Artyr Vorona
win32:RC_ICONS += sh.ico

