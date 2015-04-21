#ifndef CREATEBD_H
#define CREATEBD_H

#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>

#include "sqlqueryeditmodel.h"
#include "logreport.h"
#include "TransposeProxyModel.h"
#include "dateofunixformat.h"


class BD : public  QObject
{
    Q_OBJECT
public:

    explicit BD();
    ~BD();
    bool Create();
    QString getDatabaseVersion();
    void UpdateDataBase();


    QSqlQueryModel* ModelSettings();
    QSqlError DeleteSetting(QString name_setting);
    QString isValueSetting(QString NameSetting);


    int is_IdPokazanie(int id_list_app_usluga, DateOfUnixFormat date/*qint64 unix_date*/);
    void UpdatePokazanieHome(int id_pokazanie, int new_pokazanie);  //Изменённое показание на начало месяца
    QSqlQueryModel* ModelPokazanie(int id_apartament, int month, int year );            //показания
    QString is_NameCounter(int id_counter);  //id_pokazanie
    SqlQueryEditModel* ModelEditPokazanie(int id_apartament, int month, int year);      //редактируемые показания
    void new_pokazanie(int id_apartament, int month, int year);
    int new_pokazanie(int id_pok_old, QString value_home);                              //добавляет новое показание
                                                                                        //на сдед месяц
    //----new interface

    static void UpdateTable(QString table, QString column,QString value, QString where1, QString where2 );
    static void UpdateTable(QString table, QString column, QString value,
                     QString where_column1, QString where_value1, QString where_column2, QString where_value2);
    static QSqlError add(QString table, QStringList column,QStringList value, int mode = 0);
    static QSqlError add(QString table, QString column,QString value, int mode = 0);
    static QSqlError QueryExecute(QString str);
    static QSqlError DeleteLine(QString table, QString id_name ,int id_line);
    static QSqlError SelectFromTable(QString str, QString *var);
private:
    QSqlDatabase db;
    LogReport LogOut;
    bool RunScript(QString filename);
    void SumCount(int id_pokazanie, bool New = false);                                  //Расчёт показаний канализации

    //delete dublicate
    QStringList strL_from_query(QString str);
private slots:
    void sl_EditPokazanie(int id_pok, QString value);
    void sl_ModelPokazanieHeaderData(QAbstractTableModel* t);
signals:
    void SelectError(int error);
};

#endif // BD_H
