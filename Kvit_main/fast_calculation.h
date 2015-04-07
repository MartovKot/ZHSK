#ifndef FAST_CALCULATION_H
#define FAST_CALCULATION_H

#include <QObject>
#include "apartment.h"
#include "dateofunixformat.h"
#include "service.h"

class Fast_Calculation
{
public:
    Fast_Calculation(const int &id_home, const int &id_organization, DateOfUnixFormat date);
    ~Fast_Calculation();
    DateOfUnixFormat date() const;
    void setDate(const DateOfUnixFormat &date);
    void fullCalc();
    static double AmountToPay(int id_apart, qint64 u_date);                                       //сумма к оплате
    static double AmountForServices(int id_apart, qint64 u_date);
    static QString Debt(int id_apart, DateOfUnixFormat date);
    static QString CreditedForReport(int id_apartament, int id_usluga,  DateOfUnixFormat date);

private:
    int m_idHome;
    int m_idOrganization;
    QList<int> m_listIdApartment;
    DateOfUnixFormat m_date;
    QString calcOfService(const QStringList &row);
    QString calcOfCounters(const QStringList &row);
    void createTempTableCredited(const QList<QStringList> &table);
    void recordInDB_CredOfApart(const QList<QStringList> &table);

    void calcOfDebt();
};

#endif // FAST_CALCULATION_H
