#include "service.h"

Service::Service(int id)
{
    idService = id;
    QString str;
    QString out;

    str = "SELECT type_usluga FROM usluga WHERE id_usluga=%1";
    str = str.arg(idService);

    db.SelectFromTable(str, &out);

    setIdType(out.toInt());

    str = "SELECT name FROM usluga WHERE id_usluga=%1";
    str = str.arg(idService);

    db.SelectFromTable(str, &out);

    setName(out);
}

Service::~Service()
{

}
int Service::getIdType() const
{
    return idType;
}

void Service::setIdType(int value)
{
    idType = value;
}
QString Service::getName() const
{
    return name;
}

void Service::setName(const QString &value)
{
    name = value;
}



