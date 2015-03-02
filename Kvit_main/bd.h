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
    QString is_DatabaseVersoin();
    void UpdateDataBase();


    QString is_NameCounter(int id_counter);

    // Функции перенести в класс объединяющий дом организацию
    QStringList sum_app(int id_org, int id_home);               //Возвращает список имён квартир в доме
    QList<int> is_ListIdApartament(int id_org, int id_home);    // Возвращает список ид квартир в доме
    //-------------------------


    int is_TypeUsluga(int id_usluga);                           //возвращает ид типа услуги
    int is_idListAppUsluga(int id_apartament, int id_usluga);

    int is_Pokazanie(int id_list_app_usluga, QDate date);       //для бланка
    int is_IdPokazanie(int id_list_app_usluga, DateOfUnixFormat date/*qint64 unix_date*/);

    QString is_NameService(int id_service);

    void UpdatePokazanieHome(int id_pokazanie, int new_pokazanie);  //Изменённое показание на начало месяца
    void UpdateHome(int id_home, QString home);


    double CreditedForReport(int id_apartament, int id_usluga,  DateOfUnixFormat date);

    QSqlQueryModel* Model(QString table);                                               //модель для ComboBox
    QSqlQueryModel* ModelUslugiTabl(int id_apartament);                                 //услуги по квартире
    QSqlQueryModel* ModelTarifTabl(int year,int month);                                 //тарифы на месяц
    QSqlQueryModel* ModelPokazanie(int id_apartament, int month, int year );            //показания
    QSqlQueryModel* ModelSettings();
    SqlQueryEditModel* ModelEditPokazanie(int id_apartament, int month, int year);      //редактируемые показания
    QSqlQueryModel* ModelPensioner(int id_home, int id_org);
    QSqlQueryModel* ModelTypeUsluga();



    QString is_Debt(int id_apart, DateOfUnixFormat date);


    QSqlError DeletePension(int id_apart);
    QSqlError DeleteUslugaApartament(int id_list_apart_usluga);
    QSqlError DeleteSetting(QString name_setting);

    void new_pokazanie(int id_apartament, int month, int year);
    QString isValueSetting(QString NameSetting);
    int new_pokazanie(int id_pok_old, QString value_home);                              //добавляет новое показание
                                                                                        //на сдед месяц
    //----new interface
    QVariant SelectFromTable (QString str) const;
    void UpdateTable(QString table, QString column,QString value, QString where1, QString where2 );
    void UpdateTable(QString table, QString column, QString value, QString where_column1, QString where_value1, QString where_column2, QString where_value2);
    int add(QString table, QStringList column,QStringList value, int mode = 0);
    int add(QString table, QString column,QString value, int mode = 0);
    QSqlError QueryExecute(QString str);
    QSqlError DeleteLine(QString table, QString id_name ,int id_line);

    bool isElectroUsluga(int id_usluga);
    bool isElectroUslugaDay (int id_usluga);
    bool isElectroUslugaNight (int id_usluga);

    bool is_pensioner_living_alone(int id_apartament);

private:

    QSqlDatabase db;
    LogReport LogOut;

    bool RunScript(QString filename);
                                          //расчёт долга
    void SumCount(int id_pokazanie, bool New = false);                                  //Расчёт показаний канализации


    //delete dublicate
    QStringList strL_from_query(QString str);
    QVariant qVariant_from_query(QString str);
private slots:
    void sl_EditPokazanie(int id_pok, QString value);
    void sl_ModelPokazanieHeaderData(QAbstractTableModel* t);
signals:
    void SelectError(int error);


};

#endif // BD_H
