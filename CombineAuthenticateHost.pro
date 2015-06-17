#-------------------------------------------------
#
# Project created by QtCreator 2015-04-24T18:06:51
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CombineAuthenticateHost
TEMPLATE = app


SOURCES += main.cpp\
        mainform.cpp \
    companyregister.cpp \
    personadd.cpp \
    deviceadd.cpp \
    personbind.cpp \
    json/json_internalarray.inl \
    json/json_internalmap.inl \
    json/json_reader.cpp \
    json/json_value.cpp \
    json/json_valueiterator.inl \
    json/json_writer.cpp \
    searchunbindpersoninfo.cpp \
    MessageDialog/messagedialog.cpp \
    InputMethod/frminput.cpp \
    systemsetting.cpp \
    iconhelper.cpp \
    app.cpp

HEADERS  += mainform.h \
    companyregister.h \
    personadd.h \
    deviceadd.h \
    personbind.h \
    json/autolink.h \
    json/config.h \
    json/features.h \
    json/forwards.h \
    json/json.h \
    json/json_batchallocator.h \
    json/reader.h \
    json/value.h \
    json/writer.h \
    searchunbindpersoninfo.h \
    MessageDialog/messagedialog.h \
    InputMethod/frminput.h \
    systemsetting.h \
    iconhelper.h \
    app.h \
    myhelper.h

FORMS    += mainform.ui \
    companyregister.ui \
    personadd.ui \
    deviceadd.ui \
    personbind.ui \
    searchunbindpersoninfo.ui \
    MessageDialog/messagedialog.ui \
    InputMethod/frminput.ui \
    systemsetting.ui

INCLUDEPATH += $$PWD

RC_FILE=main.rc

DESTDIR=bin
MOC_DIR=temp/moc
RCC_DIR=temp/rcc
UI_DIR=temp/ui
OBJECTS_DIR=temp/obj

RESOURCES += \
    image.qrc

OTHER_FILES += \
    json/sconscript
