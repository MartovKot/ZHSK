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

bool Service::isElectro()
{
    if(idService == 4 ){
        return true;
    }
    return false;
}

bool Service::isElectroDay()
{
    if(idService == 6 ){
        return true;
    }
    return false;
}

bool Service::isElectroNight()
{
    if(idService == 7 ){
        return true;
    }
    return false;
}

QSqlQueryModel *Service::modelServiceType()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT  name,id_type_usluga FROM type_usluga";
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Тип услуги"));

    return model;
}



