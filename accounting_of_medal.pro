#-------------------------------------------------
#
# Project created by QtCreator 2020-01-29T17:05:38
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = accounting_of_medal
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
#    dlg_doc.cpp \
#    dlg_equipment.cpp \
#    dlg_organizations.cpp \
#    dlg_equipments_directory.cpp \
#    dlg_executors.cpp \
#    dlg_report.cpp \
#    dlg_report_availability.cpp \
    main.cpp \
#    mw_gl.cpp \
#    loggingcategories.cpp \
    dlg_authentication.cpp \
    mw.cpp \
    form_statistic.cpp \
    dlg_receiving.cpp \
    frm_receiwing.cpp \
    frm_sendering.cpp \
    frm_document_managering.cpp \
    frm_filter.cpp
#    exchange.cpp \
#    old_text.cpp

HEADERS += \
#    dlg_doc.h \
#    dlg_equipment.h \
#    dlg_organizations.h \
#    dlg_equipments_directory.h \
#    dlg_executors.h \
#    dlg_report.h \
#    dlg_report_availability.h \
#    mw_gl.h \
#    loggingcategories.h \
    dlg_authentication.h \
    mw.h \
    form_statistic.h \
    dlg_receiving.h \
    frm_receiwing.h \
    frm_sendering.h \
    frm_document_managering.h \
    frm_filter.h

FORMS += \
#    mw_gl.ui \
#    dlg_equipment.ui \
#    dlg_organizations.ui \
#    dlg_equipments_directory.ui \
#    dlg_executors.ui \
#    dlg_report.ui \
#    dlg_report_availability.ui \
    dlg_authentication.ui \
    mw.ui \
    form_statistic.ui \
    dlg_receiving.ui \
    frm_receiwing.ui \
    frm_sendering.ui \
    frm_document_managering.ui \
    frm_filter.ui
#    dlg_doc_old.ui \
#    dlg_doc.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    querys

