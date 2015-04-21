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
};

#endif // BD_H
