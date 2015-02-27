#ifndef TABLE_PAYMENT_H
#define TABLE_PAYMENT_H
#include "bd.h"
#include "dateofunixformat.h"

class Table_Payment
{

private:
    BD *db;
public:
    Table_Payment();
    ~Table_Payment();
    bool add_line(QString id_apartament,QString year,QString month,QString day,QString payment);
    QSqlQueryModel* ModelPayment(int id_apartament);                                        //все платежи по квартире
    QString delete_Payment(int id_apartament, int year,int month,int day);
    QSqlQueryModel* ModelPayment(int id_apartament, int month, int year);
//    QVariant SumLastMonthPayment();


};

#endif // TABLE_PAYMENT_H
