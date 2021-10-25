#ifndef PARSER_BLANK_H
#define PARSER_BLANK_H
#include <QObject>

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QProgressDialog>
#include <QThread>

#include "bd.h"
#include "table_tariff.h"
#include "dateofunixformat.h"

#include "home.h"
#include "apartment.h"
#include "fast_calculation.h"
#include "service.h"
#include "indications.h"

class parser_blank :public QWidget
{
    Q_OBJECT
public:
    parser_blank(QWidget *parent=0);
    QString isfirstfile();
    QString isnextfile();
    void setDate( int year, int month, int id_home, int id_org);
    void setDir(QString str_dirFolder);
    void generating();
    QStringList OutStrL();

private:
    struct ConfigData {
        int Org_id, Home_id;
        QDate date;
    } ConfData;

    QString str_folder;  //каталог для создания отчётов
    QString str_folder_arhiv;
    BD db;
    table_tariff tbl_tariff;
    void creat_blank(QString name, const Apartment & apartment,QDate date);
    QString process_usluga(QString str_in_usl, int id_app, QDate date);
    QString process_schet(QString str_in_sch, const int id_app, QDate date);
    QString process_main(QString str_in, const Apartment & apartment);
    QString nextMonth(int month);
    int nextMonthN(int month);

};

#endif // PARSER_BLANK_H
