TEMPLATE = app


CONFIG += windows
CONFIG -= qt

#HEADERS = \
#    copyprotection.h

SOURCES = \
    main.cpp #\
#    copyprotection.cpp

win32 {
    RC_FILE = copy-protection.rc
    OTHER_FILES = copy-protection.rc
}
