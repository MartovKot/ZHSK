#include "sqlquerymodelapartament.h"

SqlQueryModelApartament::SqlQueryModelApartament(QObject *parent)
    : SqlQueryEditModel(parent)
{
    connect(this,SIGNAL(sgn_EditApartament(int,QString)),SLOT(sl_EditIndex(int,QString)));
}

void SqlQueryModelApartament::sl_EditIndex(int col, QString value)
{
    emit sgn_EditIndex(col,value,id_apart);
}
