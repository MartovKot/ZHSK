#include "indications.h"

Indications::Indications(int idService, int idApartment, QDate date, QObject *parent) : QObject(parent)
{
    QString str;
    Apartment apartment(idApartment);
    DateOfUnixFormat unix_date(date);
    str = "SELECT id_pokazanie FROM pokazanie WHERE id_list_app_usluga=%1 AND date_pokazanie=%2";
    str = str.arg(apartment.isIdListApartamentServise(idService))
            .arg(unix_date.Second_first_day());
    QString id;
    BD::SelectFromTable(str,&id);
    m_id = id.toInt();
    m_idApartment = idApartment;
    m_idService = idService;
    m_Udate = unix_date.Second_first_day();

}

Indications::Indications(int idIndication, QObject *parent): QObject(parent)
{
    QString str;
    QString val;
    str = "SELECT COUNT(*) FROM pokazanie WHERE id_pokazanie = %1";
    str = str.arg(idIndication);
    if(BD::SelectFromTable(str,&val).number() == 0){
        if(val.toInt() != 0){
            m_id = idIndication;
        }else{
            setDefault();
            return;
        }
    }else{
        setDefault();
        return;
    }

    str = "SELECT date_pokazanie FROM pakazanie WHERE id_pokazanie = %1";
    str = str.arg(idIndication);
    BD::SelectFromTable(str,&val);
    m_Udate = val.toULongLong();

    str =   " SELECT a.id_apartament "
            " FROM pokazanie p, apartament a, list_app_usluga lau "
            " WHERE p.id_pokazanie = '%1' AND "
            " lau.id_list_app_usluga = p.id_list_app_usluga AND "
            " a.id_apartament = lau.id_apartament ";
    BD::SelectFromTable(str,&val);
    m_idApartment = val.toInt();

    str =   " SELECT u.id_usluga "
            " FROM pokazanie p, apartament a, list_app_usluga lau "
            " WHERE p.id_pokazanie = '%1' AND "
            " lau.id_list_app_usluga = p.id_list_app_usluga AND "
            " a.id_apartament = lau.id_apartament ";
    BD::SelectFromTable(str,&val);
    m_idService = val.toInt();

}

Indications::Indications(QObject *parent): QObject(parent)
{
    setDefault();
}


int Indications::valueIndicationHome()
{
    QString str;
    QString out;

    str = "SELECT pokazanie_home FROM pokazanie WHERE id_pokazanie='%1' ";
    str = str.arg(m_id);
    BD::SelectFromTable(str,&out);

    return out.toInt();
}

void Indications::UpdateHome(int newValue)
{
    BD::UpdateTable("pokazanie",
                    "pokazanie_home",
                    QString::number(newValue),
                    "id_pokazanie",QString::number(m_id));
    calcSewerage(true);
}

int Indications::New(int id_pok_old, QString value_home)
{
    int id_new = -1;
    int date_old = -1, id_ListApart=-1;
    QString str;
    QStringList column, value;
    QSqlQuery query;

    //  найдём дату текущих показаний
    str = "SELECT date_pokazanie, id_list_app_usluga FROM pokazanie WHERE id_pokazanie=%1";
    str = str.arg(id_pok_old);

    if (query.exec(str)){
        if (query.next()){
            date_old =  query.value(0).toInt();
            id_ListApart = query.value(1).toInt();
        }else{
            qDebug() << "not record" << "575af8aa8da14fec062ce2bc5ef9e1e8 " << str;
        }
    } else{
            qDebug()<<query.lastError();
    }
    QDateTime time;
    time = time.fromTime_t(date_old);
    time = time.addMonths(1);
    DateOfUnixFormat date(time.date());

    //Проверим на существование показаний на след месяц
    str = "SELECT id_pokazanie FROM pokazanie "
            "WHERE date_pokazanie=%1 AND id_list_app_usluga=%2";
    str = str.arg(date.Second())
            .arg(QString::number(id_ListApart));
    QString id_pok_var;
    BD::SelectFromTable(str,&id_pok_var);
    if(id_pok_var == ""){//если нет записей
        //добавим новое показание на след месяц
        column.clear();
        column<<"id_list_app_usluga"<<"date_pokazanie"<<"pokazanie_home"<<"pokazanie_end";
        value.append(QString::number(id_ListApart));
        value.append(QString::number(date.Second()));
        value.append(value_home);
        value.append(QString::number(0));
        if (BD::add("pokazanie",column,value).number() != 0){
            return -1;
        }
    }else{//иначе
        //Обновим запись
        BD::UpdateTable("pokazanie","pokazanie_home",value_home,"id_pokazanie",id_pok_var);
        return id_pok_var.toInt();
    }
    return id_new;
}

void Indications::New(int id_apartament, int month, int year)
{
    QString str;
    QSqlQuery query;
    DateOfUnixFormat date(year,month,1);
    str =   " SELECT id_pokazanie, pokazanie_end "
            " FROM pokazanie p, list_app_usluga lau "
            " WHERE lau.id_apartament = %1 "
                " AND lau.id_list_app_usluga = p.id_list_app_usluga "
                " AND p.date_pokazanie = %2 ";
    str = str.arg(id_apartament)
                .arg(QString::number(date.Second()));

    if (query.exec(str)) {
        while (query.next()) {
            New(query.value(0).toInt(),query.value(1).toString());
        }
    }else{
        qDebug()<<query.lastError();
    }
}

SqlQueryEditModel *Indications::ModelEditPokazanie(int id_apartament, int month, int year)
{
    SqlQueryEditModel *model = new SqlQueryEditModel;
    connect(model,SIGNAL(sgn_EditPokazanie(int,QString)),this,SLOT(slEdit(int,QString)));
    connect(model,SIGNAL(sgn_RefreshModel(QAbstractTableModel*)),this,SLOT(slModelPokazanieHeaderData(QAbstractTableModel*)));
    QList<int> lst;
    lst << 2;
    model->setEditColumn(lst);

    DateOfUnixFormat date(year,month,1);
    QString str;
    str = "SELECT p.id_pokazanie, u.name, p.pokazanie_end"
            " FROM list_app_usluga lau, usluga u, tariff t, pokazanie p "
            " WHERE "
            " lau.id_usluga = u.id_usluga "
            " AND t.id_usluga = u.id_usluga "
            " AND u.type_usluga = 1 "
            " AND NOT u.id_usluga = 3"
            " AND lau.id_apartament="+QString::number(id_apartament)+" "
            " AND p.id_list_app_usluga=lau.id_list_app_usluga "
            " AND t.tariff_date=p.date_pokazanie "
            " AND p.date_pokazanie="+QString::number(date.Second());
    model->setMyQuery(str);
    slModelPokazanieHeaderData(model);
    return model;
}

QSqlQueryModel *Indications::ModelPokazanie(int id_apartament, int month, int year)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    DateOfUnixFormat date(year,month,1);
    str = "SELECT p.id_pokazanie, u.name, p.pokazanie_home, p.pokazanie_end, t.tariff, t.tariff2, t.norm "
            "FROM list_app_usluga lau, usluga u, tariff t, pokazanie p "
            "WHERE "
            "lau.id_usluga=u.id_usluga "
            "AND t.id_usluga=u.id_usluga "
            "AND u.type_usluga=1 "
            "AND lau.id_apartament="+QString::number(id_apartament)+" "
            "AND p.id_list_app_usluga=lau.id_list_app_usluga "
            "AND t.tariff_date=p.date_pokazanie "
            "AND p.date_pokazanie="+QString::number(date.Second());
    model->setQuery(QSqlQuery(str));

    if(model->lastError().number() != -1){
        qDebug()<<"e825d464c306efe892a28669bdcefb13"<<model->lastError();
    }

    if(model->rowCount()==0){//  если строки не найдены, скорее всего нет тарифов - сделаем без них
        QString str2 = "SELECT p.id_pokazanie, u.name, p.pokazanie_home, p.pokazanie_end "
            "FROM list_app_usluga lau, usluga u, pokazanie p "
            "WHERE "
            "lau.id_usluga=u.id_usluga "
            "AND u.type_usluga=1 "
            "AND lau.id_apartament="+QString::number(id_apartament)+" "
            "AND p.id_list_app_usluga=lau.id_list_app_usluga "
            "AND p.date_pokazanie="+QString::number(date.Second());
        model->setQuery(QSqlQuery(str2));
    }
    if(model->lastError().number() != -1){
        qDebug()<<"e825d464c306efe823628669bdcefb13"<<model->lastError();
    }

    if(model->rowCount()==0){  //ООо всё равно пусто, то тогда добавим строчки
        QString str2;
        QSqlQuery query2;
        QStringList column, values;

        column << "id_list_app_usluga" << "date_pokazanie"
               << "pokazanie_home"     << "pokazanie_end";

        str2 = "SELECT id_list_app_usluga FROM list_app_usluga WHERE id_apartament=%1"; //все Услуги квартиры
        str2 = str2.arg(id_apartament);

        if (query2.exec(str2)){
            if (query2.size() == -1){
                return model;
            }
            while (query2.next()){ // переберём все строчки
                values.clear();
                values << query2.value(0).toString() << QString::number(date.Second())
                       << QString::number(0)         << QString::number(0);
                BD::add("pokazanie",column,values);
            }
        } else{
            qDebug()<<query2.lastError();
        }

        model = ModelPokazanie(id_apartament, month, year); // И теперь попробуем ещё раз получить не пустую модель
    }
    if(model->lastError().number() != -1){
        qDebug()<<"e8237664c306efe892a28669bdcefb13"<<model->lastError();
    }
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Счётчик"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Пок посл"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Пок тек"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("Тариф"));
    model->setHeaderData(5,Qt::Horizontal,QObject::trUtf8("Тариф2"));
    model->setHeaderData(6,Qt::Horizontal,QObject::trUtf8("Норма"));

    return model;
}

QString Indications::getNameService()
{
    QString str;
    QString out;

    str = " SELECT u.name FROM pokazanie p, list_app_usluga lau, usluga u "
            " WHERE  p.id_pokazanie = %1"
            " AND lau.id_usluga = u.id_usluga"
            " AND p.id_list_app_usluga = lau.id_list_app_usluga";
    str = str.arg(m_id);
    BD::SelectFromTable(str,&out);

    return out;
}

Indications::~Indications()
{

}

void Indications::setDefault()
{
    m_idService = -1;
    m_idApartment = -1;
    m_id = -1;
    m_Udate = -1;
}

void Indications::calcSewerage(bool newIndicator)
{
    QString str;
    QSqlQuery query;
    int value = 0;

    if (!newIndicator){//Если нет метки нового счётчика
        str = " SELECT SUM(pokazanie_end) ";                              //показания за дату у квартиры
    }else{
        str = " SELECT SUM(pokazanie_home) ";
    }
    str +=  " FROM pokazanie p, list_app_usluga lau "
            " WHERE date_pokazanie = %1 "
            " AND id_apartament = %2 "
            " AND lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND (id_usluga=1 OR id_usluga=2)";                // Воды и ГВС

    str = str.arg(m_Udate)
                .arg(m_idApartment);

    QString out;
    if(BD::SelectFromTable(str,&out).number() != 0 ){
        return;
    }

    value += out.toInt();

    if (!newIndicator){
        str = " SELECT id_pokazanie, pokazanie_end ";
    } else {
        str = " SELECT id_pokazanie, pokazanie_home ";
    }
    str +=  " FROM pokazanie p, list_app_usluga lau "
            " WHERE lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND date_pokazanie = %1 "
            " AND id_apartament = %2"
            " AND id_usluga = 3";                           //ид счётчика канализации

    str = str.arg(m_Udate)
            .arg(m_idApartment);

    if (query.exec(str)){
        if (query.next()){
            int id_sumpok;
            id_sumpok = query.value(0).toInt();
            if (query.value(1).toInt() != value){
                if(!newIndicator){
                    BD::UpdateTable("pokazanie","pokazanie_end",
                                QString::number(value),"id_pokazanie",QString::number(id_sumpok));
                } else {
                    BD::UpdateTable("pokazanie","pokazanie_home",
                                QString::number(value),"id_pokazanie",QString::number(id_sumpok));
                }
            }
            if (!newIndicator){
                New(id_sumpok,QString::number(value)); //показание на начало след месяца
            }
        }
    }else{
        qDebug()<<query.lastError();
    }
}

void Indications::slEdit(int id_pok, QString value)
{
    QSqlQuery query;
    query.prepare("UPDATE pokazanie set pokazanie_end = ? WHERE id_pokazanie = ?");
    query.addBindValue(value);
    query.addBindValue(id_pok);
    if (!query.exec())
    {
        qDebug()<<query.lastError()<<"\n";
    }
    calcSewerage(id_pok); //канализация
    New(id_pok,value); //новое показание на след месяц
}

void Indications::slModelPokazanieHeaderData(QAbstractTableModel *t)
{
    t->setHeaderData(0, Qt::Horizontal, QObject::trUtf8("№"));
    t->setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Счётчик"));
    t->setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Показазия текущие"));
}

