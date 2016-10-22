#include "apartment.h"

Apartment::Apartment(int id_home, int id_org, int number, QObject *parent):
    QObject(parent)
{
    m_number = number;

    QString str;
    QString id;
    str = "SELECT id_apartament FROM apartament WHERE id_homes='%1' AND id_organiz='%2' AND number='%3'";
    str = str.arg(id_home)
           .arg(id_org)
           .arg(number);
    BD::SelectFromTable(str, &id);
    if(id == ""){
        setDefault();
    }else{
        m_id = id.toInt();
    }

    islivingAlonePensioner = isPensionerLivingAlone();
}

Apartment::Apartment(int id_apartment, QObject *parent):
    QObject(parent)
{
    QString str;
    QString number;
    str = "SELECT number FROM apartament WHERE id_apartament='%1'";
    str = str.arg(id_apartment);
    BD::SelectFromTable(str, &number);
    if(number == ""){
        setDefault();
    }else{
        m_id = id_apartment;
        m_number = number.toInt();
    }
    islivingAlonePensioner = isPensionerLivingAlone();
}

Apartment::~Apartment()
{
}

int Apartment::getId() const
{
    return m_id;
}

int Apartment::getNumber() const
{
    return m_number;
}

bool Apartment::New(int id_home, int id_org, int number)
{
    QStringList column, value;

    column<<"id_homes"<<"id_organiz"<<"number";

    value << QString::number(id_home) << QString::number(id_org) << QString::number(number);
    if (BD::add("apartament",column,value).number() == 0){
        return true;
    }
    return false;
}

QSqlQueryModel* Apartment::ModelAllApartment(int id_home, int id_org)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT number FROM apartament "
            " WHERE id_homes = " + QString::number(id_home) +
            " AND id_organiz = " + QString::number(id_org);
    model->setQuery(QSqlQuery(str));

    return model;
}

QSqlQueryModel *Apartment::ModelAllApartmentNumFIO(int id_home, int id_org)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT number || ' - ' || surname || ' ' || SUBSTR(name,1,1) || '. ' || SUBSTR(patronymic,1,1) || '.'"
            " FROM apartament "
            " WHERE id_homes = " + QString::number(id_home) +
            " AND id_organiz = " + QString::number(id_org);
    model->setQuery(QSqlQuery(str));

    return model;
}

QSqlQueryModel *Apartment::ModelPensionerLivingAlone(int id_home, int id_org)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;

    str = "SELECT pla.id_apartament,"
            " a.surname || ' ' || a.name || ' ' || a.patronymic, "
            " a.number "
            " FROM pensioner_living_alone pla, apartament a "
            " WHERE a.id_apartament = pla.id_apartament "
            " AND a.id_organiz = %1"
            " AND a.id_homes = %2";
    str = str.arg(id_org)
            .arg(id_home);
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("ID"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("ФИО"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("№ Квартиры"));
    return model;
}

int Apartment::getMenInApartment(QString typeofuse, DateOfUnixFormat date) const
{
    QString str;
    QString out;

    str =   " SELECT " + typeofuse + " FROM men_in_apartament "
            " WHERE id_apartament=%1 AND date_men_in_apartament <= %2"
            " ORDER BY date_men_in_apartament DESC "
            " LIMIT 1 ";
    str = str.arg(m_id)
            .arg(date.Second());
    BD::SelectFromTable(str,&out);

    return out.toInt();
}

void Apartment::setDefault()
{
    m_id = -1;
    m_number = -1;
}
bool Apartment::getIslivingAlonePensioner() const
{
    return islivingAlonePensioner;
}

void Apartment::setIslivingAlonePensioner(bool value)
{
    if(value == false){
        setNoPensionerLivingAlone();
    }else{
        setYesPensionerLivingAlone();
    }
    islivingAlonePensioner = isPensionerLivingAlone();

}

bool Apartment::deleteService(QString nameService)
{
    QString str;

    str = " DELETE FROM list_app_usluga WHERE  id_list_app_usluga ='%1' ";
    str = str.arg(isIdListApartamentServise(nameService));

    if (BD::QueryExecute(str).number() == 0){
        return true;
    }
    return false;
}

int Apartment::isIdListApartamentServise(QString nameService)
{
    QString str;
    QString out;

    str =   " SELECT lau.id_list_app_usluga FROM "
            " list_app_usluga lau, usluga u, apartament a "
            " WHERE lau.id_apartament = a.id_apartament AND "
            " lau.id_usluga = u.id_usluga AND "
            " u.name = '%1' AND "
            " a.id_apartament = '%2'";
    str = str.arg(nameService)
            .arg(m_id);

    BD::SelectFromTable(str, &out);

    return out.toInt();
}

int Apartment::isIdListApartamentServise(int idService)
{
    QString str;
    QString out;

    str = "SELECT id_list_app_usluga FROM list_app_usluga WHERE id_usluga=%1 AND id_apartament=%2";
    str = str.arg(idService)
            .arg(m_id);

    BD::SelectFromTable(str, &out);

    return out.toInt();
}

int Apartment::getIdOrganization() const
{
    QString str;
    QString out;

    str = "SELECT id_organiz FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str, &out);

    return out.toInt();
}


void Apartment::DeleteApartment()
{
    BD::DeleteLine("apartament","id_apartament",m_id);
}

QString Apartment::is_FIO_payer() const
{
    QString str;
    QString out;


    str = "SELECT  surname || ' ' || name || ' ' || patronymic   FROM apartament WHERE id_apartament = %1";
    str = str.arg(m_id);

    BD::SelectFromTable(str, &out);

    return out;
}

double Apartment::getTotalArea() const
{
    QString str;
    QString out;

    str = "SELECT total_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toDouble();
}

double Apartment::getHeatedArea() const
{
    QString str;
    QString out;

    str = "SELECT total_area + balkon * heated FROM \
            (SELECT total_area, balkon, CASE WHEN hl.id_apartament IS NULL THEN 0 ELSE 1 END as heated \
            FROM apartament a \
            LEFT JOIN heated_loggia hl ON hl.id_apartament = a.id_apartament  \
            WHERE a.id_apartament = %1)";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toDouble();
}

double Apartment::getLivedArea() const
{
    QString str;
    QString out;

    str = "SELECT inhabed_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toDouble();
}


double Apartment::getLodjia() const
{
    QString str;
    QString out;

    str = "SELECT loggia FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toDouble();
}

int Apartment::getPersonalAccount() const
{
    QString str;
    QString out;

    str = "SELECT personal_account FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toInt();
}

QList<int> Apartment::getListIdServiceWithCounter()
{
    QString str;
    QSqlQuery query;
    QList<int> out;

    str = "SELECT lau.id_usluga "
            "FROM usluga u, list_app_usluga lau "
            "WHERE u.id_usluga=lau.id_usluga AND u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(m_id);

    if (!query.exec(str)){
        qDebug() << "92df4946066ff2f748cbd0178e263cac" << "Eror in "<<str<<query.lastError();
    } else {
        while (query.next()){
            out << query.value(0).toInt();
        }
    }
    return out;
}

QList<int> Apartment::getListIdServiceOutCounter()
{
    QString str;
    QSqlQuery query;
    QList<int> out;

    str = "SELECT lau.id_usluga FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND NOT u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(m_id);

    if (!query.exec(str)){
        qDebug() << "657e96603000538d92a0d97f98694612"<< "Eror in "<<str<<query.lastError();
    } else {
        while (query.next()){
            out << query.value(0).toInt();
        }
    }
    return out;
}

QList<int> Apartment::getListIdServiceFull()
{
    QList<int> out;   //список ид услуг относящихся к этой квартире

    QString str;
    QSqlQuery query;

    str = "SELECT id_usluga FROM list_app_usluga WHERE id_apartament=%1";
    str = str.arg(m_id);

    if (query.exec(str)){
      while (query.next()){
          out <<  query.value(0).toInt();
      }
    } else{
        qDebug()<<query.lastError();
    }

    return out;
}

double Apartment::getBalkon() const
{
    QString str;
    QString out;

    str = "SELECT balkon FROM apartament WHERE id_apartament=%1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&out);

    return out.toDouble();
}

int Apartment::getRealMen(DateOfUnixFormat date) const
{
    return getMenInApartment("real_men",date);
}

int Apartment::getRentMen(DateOfUnixFormat date) const
{
    return getMenInApartment("rent_men",date);
}

int Apartment::getReservMen(DateOfUnixFormat date) const
{
    return getMenInApartment("reserv_men",date);
}

bool Apartment::isPensionerLivingAlone()
{
    QString str;
    QString count;

    str = "SELECT COUNT(*) FROM pensioner_living_alone WHERE id_apartament = %1";
    str = str.arg(m_id);

    BD::SelectFromTable(str,&count);

    if (count.toInt()>0){
        return true;
    }

    return false;
}

QSqlError Apartment::setNoPensionerLivingAlone()
{
    return BD::DeleteLine("pensioner_living_alone","id_apartament",m_id);
}

QSqlError Apartment::setYesPensionerLivingAlone()
{
    return BD::add("pensioner_living_alone","id_apartament",QString::number(m_id));
}
