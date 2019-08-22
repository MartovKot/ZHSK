#ifndef FAST_CALCULATION_H
#define FAST_CALCULATION_H

#include <QObject>
#include "apartment.h"
#include "dateofunixformat.h"
#include "service.h"
#include "home.h"
#include "table_tariff.h"

class Fast_Calculation: public QObject
{
public:
    Fast_Calculation(const int &id_home, DateOfUnixFormat date);
    ~Fast_Calculation();
    DateOfUnixFormat date() const;
    void setDate(const DateOfUnixFormat &date);
    void fullCalc();
    static double AmountToPay(int id_apart, qint64 u_date);                                       //сумма к оплате
    static double AmountForServices(int id_apart, qint64 u_date);
    static QString Debt(int id_apart, DateOfUnixFormat date);
    static QString Volume(int id_apartament, int id_usluga,  DateOfUnixFormat date);
    static QString Unit(int id_apartament, int id_usluga,  DateOfUnixFormat date);

    static QString CreditedForReport(int id_apartament, int id_usluga,  DateOfUnixFormat date);



private:
    Home *home;
    DateOfUnixFormat m_date;
    QString calcOfService(const QStringList &row);
    QString calcOfCounters(const QStringList &row);
    static QString serviceVolume_type1(int id_apartament, int id_usluga,  DateOfUnixFormat date);
    void createTempTableCredited(const QList<QStringList> &table);
    void recordInDB_CredOfApart(const QList<QStringList> &table);
    void calcOfDebt();
    static double mDebt(int idApart, qint64 uDate);
};

#endif // FAST_CALCULATION_H
