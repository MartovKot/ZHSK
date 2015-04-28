#ifndef APARTMENT_H
#define APARTMENT_H

#include <QObject>
#include "bd.h"

#include "editapartmentmodel.h"

class Apartment: public QObject
{
    Q_OBJECT
public:
    explicit Apartment();
    explicit Apartment(int id_home,int id_org,int number);

    int getId() const;
    bool New(int id_home, int id_org, int number);
    void setIdAndNum(int id_home, int id_org, int number);
    static QSqlQueryModel* ModelAllApartment(int id_home, int id_org); //модель квартир для ComboBox
    QAbstractItemModel* ModelOneApartment(int id);     //модель для квартиры
    void DeleteApartment();
    QString is_FIO_payer() const;
//    static QStringList AllApartmentNumber(int id_org, int id_home);

private:
    BD db;
    int m_id;
    int m_number;
    void UpdateApartament(QStringList column,QStringList value, int idapart);   //Обновление таблицы с квартирами
    void UpdateApartament(QString column,QString value, int idapart);           //Обновление таблицы с квартирами
    void UpdateMenInApartment(QString column,QString value, int idapart);
    void AddLineMenInApartment(int id_apartment);

private slots:
    void sl_ModelApartamentHeaderData(QAbstractTableModel* t);
    void sl_EditApartament(int,QString);

signals:
    void sgn_EditModel();

};

#endif // APARTMENT_H
