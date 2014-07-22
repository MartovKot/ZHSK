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

    QString is_NameCounter(int id_counter);

    QStringList sum_app(int id_org, int id_home);               //Возвращает список имён квартир в доме

    QStringList Sum_Schet(int id_apartament);                   //Возвращает список счётчиков
    QStringList Sum_Service(int id_apartament);                 //Возвращает список услуг


    int is_TypeUsluga(int id_usluga);                           //возвращает ид типа услуги
    int is_idListAppUsluga(int id_apartament, int id_usluga);

    int is_LSh(int id_app);                                     //возвращает лицевой счёт квартиры

    int is_RealMen(int id_app, DateOfUnixFormat date);                     //возвращает количество проживающих.
    int is_RentMen(int id_app, DateOfUnixFormat date);                     //возвращает количество снимающих
    int is_ReservMen(int id_app, DateOfUnixFormat date);                   //возвращает количество на брони


    int is_Pokazanie(int id_list_app_usluga, QDate date);       //для бланка
    int is_IdPokazanie(int id_list_app_usluga, DateOfUnixFormat date/*qint64 unix_date*/);

    double is_TotalArea(int id_app);
    double is_LivedArea(int id_app);
    double is_Balkon(int id_app);
    double is_Lodjia(int id_app);
    int is_NumberAppartament(int id_app);
    QString is_DatabaseVersoin();

    QList<int> is_ApartamentService(int id_app);                //Возвращает список ид услуг по квартире
    QList<int> is_ListIdServiceWithCounter(int id_apartament);
    QList<int> is_ListIdServiceOutCounter(int id_apartament);
    QList<int> is_ListIdApartament(int id_org, int id_home);


    QString is_NameService(int id_service);

//    void UpdateOrganization(QString name = "", QString bank="", QString acc="", QString inn="", int idorg = -1);
    void UpdatePokazanieHome(int id_pokazanie, int new_pokazanie);  //Изменённое показание на начало месяца
    void UpdateHome(int id_home, QString home);

    void CreditedOfService (int id_apartament, DateOfUnixFormat date);                             //начисление по квартире
    double CreditedForReport(int id_apartament, int id_usluga,  DateOfUnixFormat date);


    QSqlQueryModel* Model(QString table);                                               //модель для ComboBox
    QSqlQueryModel* ModelUslugiTabl(int id_apartament);                                 //услуги по квартире
    QSqlQueryModel* ModelTarifTabl(int year,int month);                                 //тарифы на месяц
    QSqlQueryModel* ModelPokazanie(int id_apartament, int month, int year );            //показания
    QSqlQueryModel* ModelSettings();
    SqlQueryEditModel* ModelEditPokazanie(int id_apartament, int month, int year);      //редактируемые показания
    QSqlQueryModel* ModelPensioner(int id_home, int id_org);


    double AmountToPay(int id_apart, qint64 u_date);                                       //сумма к оплате
    QString is_Debt(int id_apart, DateOfUnixFormat date);
    double AmountForServices(int id_apart, qint64 u_date);

    QSqlError DeletePension(int id_apart);
    QSqlError DeleteUslugaApartament(int id_list_apart_usluga);
    QSqlError DeleteSetting(QString name_setting);

    void UpdateDataBase();
    void new_pokazanie(int id_apartament, int month, int year);
    QString isValueSetting(QString NameSetting);
    int new_pokazanie(int id_pok_old, QString value_home);                              //добавляет новое показание
                                                                                        //на сдед месяц
    //----new interface
    QVariant SelectFromTable (QString str) const;
    void UpdateTable(QString table, QString column,QString value, QString where1, QString where2 );
    int add(QString table, QStringList column,QStringList value);
    int add(QString table, QString column,QString value);
    void DeleteLine(QString table, QString id_name ,int id_line);


private:

    QSqlDatabase db;
    LogReport LogOut;

    bool RunScript(QString filename);

    QVariant CreditedOfApartament(int id_list_app_usluga, DateOfUnixFormat date);
    void CreditedForApartament(int id_apart, DateOfUnixFormat date);
    void PaymentOfDebt(int id_apart, int year, int month/*DateOfUnixFormat date*/);                                       //расчёт долга
    double PaymentCounters(int id_list_app_usluga, DateOfUnixFormat date);

    void SumCount(int id_pokazanie, bool New = false);                                  //Расчёт показаний канализации

    bool isElectroUsluga(int id_usluga);
    bool is_pensioner_living_alone(int id_apartament);

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
