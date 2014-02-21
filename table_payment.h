#ifndef TABLE_PAYMENT_H
#define TABLE_PAYMENT_H

#include "bd.h"

class Table_Payment
{
private:
    BD *db;
    qint64 IsDateOfUnix(int year, int month, int day)
    {
        qint64 timeInUnix;
        const qint64 MS_COEF = 1000;
        QDate date;
        date.setDate(year, month, day);
        QDateTime datetime;
        datetime.setTimeSpec(Qt::OffsetFromUTC);
        datetime.setDate(date);
        timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;
        return timeInUnix;
    }
public:
    Table_Payment();
    ~Table_Payment();
    bool add_line(QString id_apartament,QString year,QString month,QString day,QString payment);
    QSqlQueryModel* ModelPayment(int id_apartament);                                        //все платежи по квартире
    QString delete_Payment(int id_apartament, int year,int month,int day);
    QSqlQueryModel* ModelPayment(int id_apartament, int month, int year);            //показания


};

#endif // TABLE_PAYMENT_H
