#ifndef APARTMENT_H
#define APARTMENT_H

#include <QObject>
#include "bd.h"

#include "editapartmentmodel.h"

class Apartment: public QObject
{
    Q_OBJECT
public:
    explicit Apartment(int id_home,int id_org,int number);
    explicit Apartment(int id_apartment);
    ~Apartment();

    static QSqlQueryModel* ModelAllApartment(int id_home, int id_org); //модель квартир для ComboBox

    int getId() const;
    int getNumber() const;
    bool New(int id_home, int id_org, int number);

    QAbstractItemModel* ModelOneApartment(int id);     //модель для квартиры
    void DeleteApartment();
    QString is_FIO_payer() const;
    double getTotalArea() const;
    double getLivedArea() const;
    double getBalkon() const;
    double getLodjia() const;
    int getPersonalAccount() const;  //возвращает лицевой счёт квартиры
    QList<int> getListIdServiceWithCounter();
    QList<int> getListIdServiceOutCounter();
    QList<int> getListIdServiceFull();                              //Возвращает список ид услуг по квартире
    int getRealMen(DateOfUnixFormat date) const;                     //возвращает количество проживающих.
    int getRentMen(DateOfUnixFormat date) const;                     //возвращает количество снимающих
    int getReservMen(DateOfUnixFormat date) const;                   //возвращает количество на брони
    bool isPensionerLivingAlone();

private:
    int m_id;
    int m_number;
    void UpdateApartament(QStringList column,QStringList value, int idapart);   //Обновление таблицы с квартирами
    void UpdateApartament(QString column,QString value, int idapart);           //Обновление таблицы с квартирами
    void UpdateMenInApartment(QString column,QString value, int idapart);
    void AddLineMenInApartment(int id_apartment);
    int getMenInApartment(QString typeofuse,DateOfUnixFormat date) const;
    void setDefault();

private slots:
    void sl_ModelApartamentHeaderData(QAbstractTableModel* t);
    void sl_EditApartament(int,QString);

signals:
    void sgn_EditModel();

};

#endif // APARTMENT_H
