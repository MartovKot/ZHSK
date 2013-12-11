######################################################################
# Automatically generated by qmake (2.01a) ?? 10. ??? 18:18:11 2013
######################################################################
QT       += webkit\
            core gui\
            sql

TEMPLATE = app
TARGET = kvit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


# Input
HEADERS += adminwindow.h \
           apartament.h \
           bd.h \
           mainwindow.h \
           message.h \
           operwindow.h \
           parser_blank.h \
           sqlqueryeditmodel.h \
    myitemdelegate.h \
    tableviewpokazanie.h \
    logreport.h \
    newcounter.h \
    viewblank.h \
    table_payment.h \
    table_tariff.h \
    my_function.h \
    table_pokazanie.h

FORMS += adminwindow.ui mainwindow.ui operwindow.ui \
    newcounter.ui
SOURCES += adminwindow.cpp \
           apartament.cpp \
           bd.cpp \
           main.cpp \
           mainwindow.cpp \
           operwindow.cpp \
           parser_blank.cpp \
           sqlqueryeditmodel.cpp \
    myitemdelegate.cpp \
    tableviewpokazanie.cpp \
    logreport.cpp \
    newcounter.cpp \
    viewblank.cpp \
    table_payment.cpp \
    table_tariff.cpp \
    table_pokazanie.cpp

RESOURCES += \
    qrc.qrc
RC_FILE = ICON.rc

OTHER_FILES += \
    logika.txt \
    future.txt \
    exaple_qss.txt \
    update_db/baseline.sql \
    update_db/01_06_001.sql \
    update_db/01_06_002.sql \
    update_db/01_06_003.sql
