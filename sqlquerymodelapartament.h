#ifndef SQLQUERYMODELAPARTAMENT_H
#define SQLQUERYMODELAPARTAMENT_H

#include "sqlqueryeditmodel.h"

class SqlQueryModelApartament :public SqlQueryEditModel
{
    Q_OBJECT

public:
    SqlQueryModelApartament(QObject *parent = 0);
    void SetIdApartament(int id){
        id_apart = id;
    }

private:
    int id_apart;
signals:
    void sgn_EditIndex(int column,QString value,int id);
private slots:
    void sl_EditIndex(int, QString);
};

#endif // SQLQUERYMODELAPARTAMENT_H
