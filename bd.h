#ifndef CREATEBD_H
#define CREATEBD_H

#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDate>
#include <QFile>

#include "qsqlquerymodel.h"
#include "qsqlrelationaldelegate.h"
#include "qsqlrelationaltablemodel.h"
#include "qsqltablemodel.h"
#include "qsql.h"
#include "qsqldatabase.h"
#include "qsqldriver.h"
#include "qsqldriverplugin.h"
#include "qsqlerror.h"
#include "qsqlfield.h"
#include "qsqlindex.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
#include "qsqlresult.h"


#include "sqlqueryeditmodel.h"
#include "logreport.h"

class BD : public  QObject
{
    Q_OBJECT
public:

    BD();
    ~BD();
    void Create();

    QString is_nameOrg(int id);                                 //Возвращает название организации c банком и расч счётом по ид
    QStringList is_nameOrg();
    QString is_SmallnameOrg(int id);                            //Возвращает название организации  по ид
    QString is_nameHome(int id);                                //Возвращает название дома по ид
    QStringList is_nameHome();
    QString is_FIO(int id_app);                                 //Возвращает полное имя платильщика
    QString is_NameCounter(int id_counter);

    QStringList sum_app(int id_org, int id_home);               //Возвращает список имён квартир в доме

    QStringList Sum_Schet(int id_apartament);                   //Возвращает список счётчиков
    QStringList Sum_Service(int id_apartament);                 //Возвращает список услуг



    int is_TypeUsluga(int id_usluga);                           //возвращает ид типа услуги
    int is_idListAppUsluga(int id_apartament, int id_usluga);

    int is_idappart(int id_home, int id_org, int num);

    int is_LSh(int id_app);                                     //возвращает лицевой счёт квартиры

    int is_RealMen(int id_app, int year, int month);                                 //возвращает количество проживающих.
    int is_RentMen(int id_app, int year, int month);                                 //возвращает количество снимающих
    int is_ReservMen(int id_app, int year, int month);                               //возвращает количество на брони


    int is_Pokazanie(int id_list_app_usluga, int month, int year); //для бланка
    int is_IdPokazanie(int id_list_app_usluga, int month, int year);

    double is_TotalArea(int id_app);
    double is_LivedArea(int id_app);
    double is_Balkon(int id_app);
    double is_Lodjia(int id_app);
    int is_NumberAppartament(int id_app);
    QString is_DatabaseVersoin();




    QList<int> is_ApartamentService(int id_app);            //Возвращает список ид услуг по квартире
    QList<int> is_ListIdServiceWithCounter(int id_apartament);
    QList<int> is_ListIdServiceOutCounter(int id_apartament);
    QList<int> is_ListIdApartament(int id_org, int id_home);


    QString is_NameService(int id_service);

    int add(QString table, QStringList column,QStringList value);
    int add(QString table, QString column,QString value);

    void UpdateApartament(QStringList column,QStringList value, int idapart);           //Обновление таблицы с квартирами
    void UpdateMenInApartament(QStringList column, QStringList value, int idapart, int year, int month);

    void UpdateOrganization(QString name = "", QString bank="", QString acc="", int idorg = -1);
    void UpdatePokazanieHome(int id_pokazanie, int new_pokazanie);
    void UpdateHome(int id_home, QString home);

    void CreditedOfService(int month,int year, int id_apartament);                      // начисление по квартире
    double CreditedForReport(int month,int year, int id_apartament, int id_usluga);


    QSqlQueryModel* Model(QString table);                                               //модель для ComboBox
    QSqlQueryModel* ModelApartament(int id_apartament);                                 //модель для квартиры
    QSqlQueryModel* ModelApartament(int id_home, int id_org);                           //модель квартир для ComboBox
    QSqlQueryModel* ModelUslugiTabl(int id_apartament);                                 //услуги по квартире
    QSqlQueryModel* ModelTarifTabl(int year,int month);                                 //тарифы на месяц
    QSqlQueryModel* ModelOrganiz();
    QSqlQueryModel* ModelHome();
    QSqlQueryModel* ModelPokazanie(int id_apartament, int month, int year );            //показания
    SqlQueryEditModel* ModelEditPokazanie(int id_apartament, int month, int year);      //редактируемые показания
    QSqlQueryModel* ModelPensioner(int id_home, int id_org);

    int NewApatament(int id_org, int id_home, int num_apart);                           //новая квартира

    double AmountToPay(int id_apart, int month, int year);                              //сумма к оплате
    QString is_Debt(int id_apart, int month, int year);
    double AmountForServices(int id_apart, int month, int year);


    QString DeleteOrg(int id_org);
    QString DeleteHome(int id_home);
    QSqlError DeletePension(int id_apart);
    QSqlError DeleteUslugaApartament(int id_list_apart_usluga);

    void UpdateDataBase();
    void new_pokazanie(int id_apartament, int month, int year);
    //----new interface
    QVariant SelectFromTable (QString str);
    void UpdateTable(QString table, QString column,QString value, QString where1, QString where2 );
    int previous_month(int m);
    int previous_year(int y, int m);

private:
    qint64 IsDateOfUnix(int year, int month, int day);
    QSqlDatabase db;
    LogReport LogOut;
    bool RunScript(QString filename);

    int new_pokazanie(int id_pok_old, QString value_home);                              //добавляет новое показание
                                                                                        //на сдед месяц

    int next_month (int m);
    int next_year(int m, int y);



    double CreditedOfApartament(int month,int year, int id_list_app_usluga);
    void CreditedForApartament(int id_apart, int month, int year);
    void PaymentOfDebt(int id_apart, int month, int year);                              //расчёт долга
    double PaymentCounters(int id_list_app_usluga, int month, int year);

    void SumCount(int id_pokazanie, bool New = false);                                                    //Расчёт показаний канализации
    void DataProcessing(int id_org, int id_home, int month, int year);                  //начало оптимизированного расчёта

    bool isElectroUsluga(int id_usluga);
    bool is_pensioner_living_alone(int id_apartament);
private slots:
    void sl_EditPokazanie(int id_pok, QString value);
signals:
    void SelectError(int error);


};

#endif // BD_H
