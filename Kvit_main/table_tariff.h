#ifndef TABLE_TARIFF_H
#define TABLE_TARIFF_H

#include "bd.h"

class table_tariff
{
private:
    QSqlError AddLineTariffNewMonth(DateOfUnixFormat date);
public:
    table_tariff();
    ~table_tariff();

    static double is_Tariff(int usluga, QDate date, int tarif = 1);           //возвращает размер тарифа
    static double is_Tariff(int usluga, qint64 date, int tarif = 1);           //возвращает размер тарифа
    QSqlQueryModel* ModelTarifTabl(int year,int month);                         //тарифы на месяц
    void UpdateTarif(QString tarif,QString tarif2, QString norma, int idusl);
    int FillTarif(int month, int year);
};

#endif // TABLE_TARIFF_H
