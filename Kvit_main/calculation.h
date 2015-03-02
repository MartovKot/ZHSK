#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QVariant>
#include <QSqlQuery>
#include <QDebug>

#include "dateofunixformat.h"
#include "table_tariff.h"
#include "apartment.h"
#include "bd.h"


class Calculation : public QObject
{
    Q_OBJECT
public:
    explicit Calculation(QObject *parent = 0);
    ~Calculation();
    void CreditedOfService (int id_apartament, DateOfUnixFormat date);                             //начисление по квартире
    double AmountToPay(int id_apart, qint64 u_date);                                       //сумма к оплате
    double AmountForServices(int id_apart, qint64 u_date);
private:
    QVariant CreditedOfApartament(int id_list_app_usluga, DateOfUnixFormat date);
    double PaymentCounters(int id_list_app_usluga, DateOfUnixFormat date);
    void CreditedForApartament(int id_apart, DateOfUnixFormat date);
    void PaymentOfDebt(int id_apart, int year, int month/*DateOfUnixFormat date*/);

    BD db;

signals:

public slots:
};

#endif // CALCULATION_H
