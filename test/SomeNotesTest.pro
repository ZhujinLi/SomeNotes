QT += testlib
QT += gui widgets
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_contentmanager.cpp \
    ../src/contentmanager.cpp \
