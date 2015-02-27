######################################################################
# Automatically generated by qmake (2.01a) ?? 10. ??? 18:18:11 2013
######################################################################
QT       += webkit\
            core gui\
            sql\
            network

TEMPLATE = app
TARGET = kvit
greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets\
            printsupport\
            webkitwidgets
    DEFINES += HAVE_QT5
}
CONFIG += c++11
# Input
HEADERS += adminwindow.h \
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
    updater.h \
    mysplashscreen.h \
    editsetting.h \
    selecter_with_combobox.h \
    transposeproxymodel.h \
    dateofunixformat.h \
    organization.h \
    home.h \
    apartment.h \
    editapartmentmodel.h \
    modelusliga.h \
    dialog_with_lineedit.h \
    massoperations.h \
    progressdialog.h
FORMS += adminwindow.ui mainwindow.ui operwindow.ui \
    newcounter.ui \
    authenticationdialog.ui \
    editsetting.ui \
    selecter_with_combobox.ui \
    dialog_with_lineedit.ui \
    massoperations.ui
SOURCES += adminwindow.cpp \
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
    updater.cpp \
    mysplashscreen.cpp \
    editsetting.cpp \
    selecter_with_combobox.cpp \
    transposeproxymodel.cpp \
    dateofunixformat.cpp \
    organization.cpp \
    home.cpp \
    apartment.cpp \
    editapartmentmodel.cpp \
    modelusliga.cpp \
    dialog_with_lineedit.cpp \
    massoperations.cpp \
    progressdialog.cpp

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
    update_db/01_06_003.sql \
    update_db/01_06_004.sql \
    update_db/01_06_005.sql \
    update_db/01_06_006.sql \
    update_db/01_06_007.sql \
    script2.nsi \
    update_db/01_06_008.sql \
    blank/blank_main.html \
    blank/blank_s.html \
    blank/blank_u.html \
    update_db/01_07_001.sql \
    update_db/01_07_002.sql

DISTFILES += \
    update_db/01_07_003.sql
