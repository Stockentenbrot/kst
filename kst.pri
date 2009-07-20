OUTPUT_DIR = $$(OUTPUT_DIR)
isEmpty(OUTPUT_DIR):OUTPUT_DIR=$$PWD/build

INSTALL_PREFIX = $$(INSTALL_PREFIX)
unix {
  !mac {
    isEmpty(INSTALL_PREFIX):INSTALL_PREFIX=/usr
} }

INSTALL_LIBDIR = $$(INSTALL_LIBDIR)
unix {
  !mac {
    isEmpty(INSTALL_LIBDIR):INSTALL_LIBDIR=lib
} }

INSTALL_PREFIX=$$(DESTDIR)$$INSTALL_PREFIX

DEFINES += INSTALL_LIBDIR=\\\"$$INSTALL_LIBDIR\\\"

QT += core xml

CONFIG += silent

DEBUG_MODE = $$(KST_DEBUG_MODE)
contains(DEBUG_MODE, 1) {
  CONFIG += debug
  QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
}

OBJECTS_DIR = tmp
MOC_DIR = tmp

TOPLEVELDIR = $$PWD

INCLUDEPATH += $$TOPLEVELDIR

LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin
VERSION = 2.0.0


