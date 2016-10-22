#ifndef APARTMENT_H
#define APARTMENT_H

#include <QObject>
#include "bd.h"


class Apartment: public QObject
{
    Q_OBJECT
public:
    explicit Apartment(int id_home,int id_org,int number, QObject *parent = 0);
    explicit Apartment(int id_apartment, QObject *parent = 0);
    ~Apartment();

    static QSqlQueryModel *ModelAllApartment(int id_home, int id_org); //модель квартир для ComboBox
    static QSqlQueryModel *ModelAllApartmentNumFIO(int id_home, int id_org);

    static QSqlQueryModel *ModelPensionerLivingAlone(int id_home, int id_org);
    static bool New(int id_home, int id_org, int number);

    int getId() const;
    int getNumber() const;

    void DeleteApartment();
    QString is_FIO_payer() const;
    double getTotalArea() const;
    double getHeatedArea() const;
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


    bool getIslivingAlonePensioner() const;
    void setIslivingAlonePensioner(bool value);

    bool deleteService(QString nameService);
    int isIdListApartamentServise(QString nameService);
    int isIdListApartamentServise(int idService);

    int getIdOrganization() const;

private:
    int m_id;
    int m_number;
    int getMenInApartment(QString typeofuse,DateOfUnixFormat date) const;
    void setDefault();
    bool islivingAlonePensioner;
    bool isPensionerLivingAlone();
    QSqlError setNoPensionerLivingAlone();
    QSqlError setYesPensionerLivingAlone();

};

#endif // APARTMENT_H
