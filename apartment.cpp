#include "apartment.h"

Apartment::Apartment()
{
    m_id = -1;
    m_number = -1;
}

Apartment::Apartment(int id_home, int id_org, int number)
{
    m_number = number;

    QString str;
    str = "SELECT id_apartament FROM apartament WHERE id_homes='%1' AND id_organiz='%2' AND number='%3'";
    str = str.arg(id_home)
           .arg(id_org)
           .arg(number);
    QVariant t = db.SelectFromTable(str);
    if (!t.isNull()){
        m_id = t.toInt();
    }else{
        m_id = -1;
    }
}

int Apartment::getId() const
{
    return m_id;
}

bool Apartment::New(int id_home, int id_org, int number)
{
    QStringList column, value;

    column<<"id_homes"<<"id_organiz"<<"number";

    value << QString::number(id_home) << QString::number(id_org) << QString::number(number);
    if (db.add("apartament",column,value) == 0){
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

QAbstractItemModel* Apartment::ModelOneApartment(int id)
{
    EditApartmentModel *model = new EditApartmentModel;

    model->setQuery(" SELECT "
                          " number, surname, name, patronymic, "
                          " total_area, inhabed_area,balkon, loggia,  personal_account  "
                          " FROM apartament "
                          " WHERE id_apartament = "+QString::number(id));

    sl_ModelApartamentHeaderData(model);
//    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(sl_test()));
    connect(model,SIGNAL(sgn_EditApartament(int,QString)),this,SLOT(sl_EditApartament(int,QString)));

    return model;

}


void Apartment::sl_ModelApartamentHeaderData(QAbstractTableModel *model)
{
//    qDebug() << "sl_ModelApartamentHeaderData";
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Фамилия"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Имя"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Отчество"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("Общ. Площадь"));
    model->setHeaderData(5,Qt::Horizontal,QObject::trUtf8("Жил. Площадь"));
    model->setHeaderData(6,Qt::Horizontal,QObject::trUtf8("Балкон"));
    model->setHeaderData(7,Qt::Horizontal,QObject::trUtf8("Лоджия"));
    model->setHeaderData(8,Qt::Horizontal,QObject::trUtf8("Личн. счёт"));
}

void Apartment::sl_EditApartament(int col,QString val)
{
    QStringList lst_col;
    QStringList lst_val;

    switch(col){
        case 0:
        lst_col << "number";
        break;
        case 1:
        lst_col << "surname";
        break;
        case 2:
        lst_col << "name";
        break;
        case 3:
        lst_col << "patronymic";
        break;
        case 4:
        lst_col << "total_area";
        break;
        case 5:
        lst_col << "inhabed_area";
        break;
        case 6:
        lst_col << "balkon";
        break;
        case 7:
        lst_col << "loggia";
        break;
        case 8:
        lst_col << "personal_account";
        break;
    }

    lst_val << val;
    UpdateApartament(lst_col,lst_val,m_id);
    emit sgn_EditModel();
}

void Apartment::UpdateApartament(QStringList column, QStringList value, int idapart)
{
    for(int i=0; i<column.count();i++ ){
        db.UpdateTable("apartament",column[i],value[i],"id_apartament", QString::number(idapart));
    }
}

void Apartment::setIdAndNum(int id_home, int id_org, int number)
{
    m_number = number;

    QString str;
    str = "SELECT id_apartament FROM apartament WHERE id_homes='%1' AND id_organiz='%2' AND number='%3'";
    str = str.arg(id_home)
           .arg(id_org)
           .arg(number);
    QVariant t = db.SelectFromTable(str);
    if (!t.isNull()){
        m_id = t.toInt();
    }else{
        m_id = -1;
    }
}

void Apartment::DeleteApartment()
{
    db.DeleteLine("apartament","id_apartament",m_id);
}

QString Apartment::is_FIO_payer() const
{
    QString str;
    QString fio = "";

    str = "SELECT  surname || ' ' || name || ' ' || patronymic   FROM apartament WHERE id_apartament = %1";
    str = str.arg(m_id);

    QVariant t = db.SelectFromTable(str);
    if (!t.isNull()){
        fio = t.toString();
    }else{
        fio = "";
    }

    return fio;
}

