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
}

Apartment::~Apartment()
{
//    qDebug() << "delete" << this;
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
    if (BD::add("apartament",column,value) == 0){
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

QAbstractItemModel* Apartment::ModelOneApartment()
{
    EditApartmentModel *model = new EditApartmentModel;
    DateOfUnixFormat date(QDate::currentDate());

    model->setQuery(" SELECT "
                        " max(mia.date_men_in_apartament),"
                        " number, surname, name, patronymic, "
                        " total_area, inhabed_area,balkon, loggia, personal_account, "
                        " mia.real_men, mia.rent_men, mia.reserv_men"
                        " FROM apartament a, men_in_apartament mia "
                        " WHERE a.id_apartament = " + QString::number(m_id) + " AND "
                        " mia.id_apartament = a.id_apartament "
                        " AND mia.date_men_in_apartament <= " + QString::number(date.Second_first_day()) + " "
                        " ORDER BY mia.date_men_in_apartament"
                    );
    model->removeColumn(0);
    sl_ModelApartamentHeaderData(model);

    connect(model,SIGNAL(sgn_EditApartament(int,QString)),this,SLOT(sl_EditApartament(int,QString)));

    return model;

}


void Apartment::sl_ModelApartamentHeaderData(QAbstractTableModel *model)
{
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Фамилия"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Имя"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Отчество"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("Общ. Площадь"));
    model->setHeaderData(5,Qt::Horizontal,QObject::trUtf8("Жил. Площадь"));
    model->setHeaderData(6,Qt::Horizontal,QObject::trUtf8("Балкон"));
    model->setHeaderData(7,Qt::Horizontal,QObject::trUtf8("Лоджия"));
    model->setHeaderData(8,Qt::Horizontal,QObject::trUtf8("Личн. счёт"));
    model->setHeaderData(9,Qt::Horizontal,QObject::trUtf8("Проживает"));
    model->setHeaderData(10,Qt::Horizontal,QObject::trUtf8("Снимает"));
    model->setHeaderData(11,Qt::Horizontal,QObject::trUtf8("Бронь"));

}

void Apartment::sl_EditApartament(int col,QString val)
{
    switch(col){
    case 0:
        UpdateApartament("number",val,m_id);
        break;
    case 1:
        UpdateApartament("surname",val,m_id);
        break;
    case 2:
        UpdateApartament("name",val,m_id);
        break;
    case 3:
        UpdateApartament("patronymic",val,m_id);
        break;
    case 4:
        UpdateApartament("total_area",val,m_id);
        break;
    case 5:
        UpdateApartament("inhabed_area",val,m_id);
        break;
    case 6:
        UpdateApartament("balkon",val,m_id);
        break;
    case 7:
        UpdateApartament("loggia",val,m_id);
        break;
    case 8:
        UpdateApartament("personal_account",val,m_id);
        break;
    case 9:
        UpdateMenInApartment("real_men",val,m_id);
        break;
    case 10:
        UpdateMenInApartment("rent_men",val,m_id);
        break;
    case 11:
        UpdateMenInApartment("reserv_men",val,m_id);
        break;
    }

    emit sgn_EditModel();
}

void Apartment::UpdateApartament(QStringList column, QStringList value, int idapart)
{
    for(int i=0; i<column.count();i++ ){
        BD::UpdateTable("apartament",column[i],value[i],"id_apartament", QString::number(idapart));
    }
}

void Apartment::UpdateApartament(QString column, QString value, int idapart)
{
    BD::UpdateTable("apartament",column,value,"id_apartament", QString::number(idapart));
}

void Apartment::UpdateMenInApartment(QString column, QString value, int idapart)
{
    QString str;
    DateOfUnixFormat date(QDate::currentDate());


    str = "SELECT COUNT(*) FROM men_in_apartament WHERE id_apartament = %1 AND date_men_in_apartament = %2";
    str = str.arg(idapart)
            .arg(date.Second_first_day());

    QString count = 0;
    BD::SelectFromTable(str,&count);

    if (count.toInt() == 0){//если записей за текущий месяц нету
        AddLineMenInApartment(idapart); //добавим запись
    }
    BD::UpdateTable("men_in_apartament",column,value,
                   "date_men_in_apartament",QString::number(date.Second_first_day()),
                   "id_apartament", QString::number(idapart));

}
void Apartment::AddLineMenInApartment(int id_apartment)
{
    QString str;
    DateOfUnixFormat date_now(QDate::currentDate());

    str = "SELECT max(date_men_in_apartament) FROM men_in_apartament WHERE id_apartament = %1";
    str = str.arg(id_apartment);
    QString date;
    BD::SelectFromTable(str,&date);
    QString str2;

    str2 = " INSERT INTO men_in_apartament (id_apartament, real_men, rent_men, reserv_men, date_men_in_apartament ) "
            " SELECT id_apartament, real_men, rent_men, reserv_men, %1 "
            " FROM men_in_apartament"
            " WHERE date_men_in_apartament = %2 AND id_apartament = %3";

    str2 = str2.arg(QString::number(date_now.Second_first_day()))
            .arg(date)
            .arg(id_apartment);
    BD::QueryExecute(str2);
//    }else{
//        qDebug() << "need add code";
    //    }
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
