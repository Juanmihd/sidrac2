# Base options
TEMPLATE = app
LANGUAGE = C++

# QT modules
QT += opengl
CONFIG += thread

CONFIG += console
# Executable name
TARGET = SIDRAC2

# Directories
DESTDIR = .
UI_DIR = build/ui
MOC_DIR = build/moc
OBJECTS_DIR = build/obj

# Lib headers
INCLUDEPATH += .
INCLUDEPATH += lib

# Lib sources
SOURCES += lib/wrap/ply/plylib.cpp \
    voxel.cpp \
    configuracion.cpp \
    hebracalculoeje.cpp \
    puntocontorno.cpp
SOURCES += lib/wrap/gui/trackball.cpp
SOURCES += lib/wrap/gui/trackmode.cpp

# Compile glew
DEFINES += GLEW_STATIC
INCLUDEPATH += lib/glew/include
SOURCES += lib/glew/src/glew.c

# Awful problem with windows..
win32:DEFINES += NOMINMAX

# Input
HEADERS += mainwindow.h \
    voxel.h \
    puntocontorno.h \
    configuracion.h \
    estructurasDeDatos.h \
    hebracalculoeje.h
HEADERS += glarea.h
SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += glarea.cpp
FORMS += mainwindow.ui \
    configuracion.ui
RESOURCES += 
