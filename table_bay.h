#ifndef TABLE_BAY_H
#define TABLE_BAY_H

#include "bd.h"

class table_bay
{
private:
    BD *db;
public:
    table_bay();
    ~table_bay();
    bool add_line(QString id_apartament,QString year,QString month,QString day,QString bay);
    QSqlQueryModel* ModelBay(int id_apartament);                                        //все платежи по квартире
};

#endif // TABLE_BAY_H
