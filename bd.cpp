#include "bd.h"
#include "table_tariff.h"
#include "sqlqueryeditmodel.h"
#include "sqlquerymodelapartament.h"


BD::BD()
{
    LogOut.setFileName("out.log");
}
BD::~BD()
{
    db.close();
}

bool BD::RunScript(QString sqlfilename)
{
    QSqlQuery query;
    QFile sqlfile(sqlfilename);
    sqlfile.open(QIODevice::ReadOnly);
    if(sqlfile.isReadable()){
        QByteArray bArrSqlFile;
        bArrSqlFile = sqlfile.readAll();

        int pos = 0;
        int count = bArrSqlFile.indexOf(";",pos);
        while (count >= 0){
            QString str = bArrSqlFile.mid(pos,count - pos);
            if (!query.exec(str)) {
                qDebug() << str << query.lastError();
                LogOut.logout(query.lastError().text());
                return false;
            } else {
               // qDebug() << "Run script " << str;
            }
            pos = count+1;
            count = bArrSqlFile.indexOf(";",pos);
        }
    }else{
        qDebug() << "not read " << sqlfile.fileName();
        return false;
    }
    return true;
}

bool BD::Create()
{
    if (!RunScript("./update_db/baseline.sql")){
        qDebug() << "Error run " << "./update_db/baseline.sql";
        return false;
    }
    return true;
}

void BD::UpdateDataBase()
{
    QString str;
    QSqlQuery query;
    QString res = "";

    str = "SELECT version FROM version";
    if (query.exec(str)) {
        if (query.next()){
            res = query.value(0).toString();
        }else{
            qDebug()<<"Version not found";
            LogOut.logout("Version not found");
            return;
        }
    }else{
        qDebug() << query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    }
    query.finish();
    if (res == "1.5"){
        str = "UPDATE version SET version = '01.05.000' WHERE version = 1.5";
        if (!query.exec(str)) {
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
        }
        UpdateDataBase();
        return;
    }
//    qDebug() << res;

    QDir dir_with_sql("./update_db");
    QStringList filters;
    QStringList list_with_sql;
    QStringList list_update;
    filters << "??_??_???.sql";
    list_with_sql = dir_with_sql.entryList(filters,QDir::Files,QDir::Name);

    QString major;
    QString minor;
    QString subversion;

    major = res.left(2);
    minor = res.mid(3,2);
    subversion = res.right(3);

    for(int i = 0;i < list_with_sql.count();i++){
        if(list_with_sql.at(i).left(2).toInt() > major.toInt()){
            list_update << dir_with_sql.absoluteFilePath(list_with_sql.at(i));
        }else if(list_with_sql.at(i).left(2).toInt() == major.toInt()){
            if(list_with_sql.at(i).mid(3,2).toInt() > minor.toInt()){
                list_update << dir_with_sql.absoluteFilePath(list_with_sql.at(i));
            }else if (list_with_sql.at(i).mid(3,2).toInt() == minor.toInt()){
                if((list_with_sql.at(i).mid(6,3).toInt() > subversion.toInt())){
                    list_update << dir_with_sql.absoluteFilePath(list_with_sql.at(i));
                }
            }
        }
    }

    for (int i=0;i<list_update.count();i++){
        if (!RunScript(list_update.at(i))){
            qDebug() << "Error run " << list_update.at(i);
        }else{
            qDebug() << "run script" << list_update.at(i);
        }
    }

}

QVariant BD::SelectFromTable(QString str)  //Функция возврящает только одно поле((
{
    QSqlQuery query;
    QVariant out;
    if (query.exec(str)){
        if (query.next()){
            out = query.value(0);
        }else{
//            qDebug()<< "NOT FOUND \n" << "b15887690eebb387ab115d9e4ee2e2d0 " <<str;
//            LogOut.logout("In query " + str + " not found line ");
        }
    } else{
        qDebug() <<"ERROR \n"<< "d04157f41fd3c5c4dee3f0e0dd41baed" << query.lastError() << str;
        LogOut.logout(query.lastError().text());
    }
    qDebug() << out.isValid() << out.isNull() <<out.isDetached();
    return out;
}

int BD::add(QString table, QString column, QString value)
{
    QStringList C_sl, V_sl;
    C_sl<<column;
    V_sl<<value;
    return add(table,C_sl,V_sl);
}

int BD::add(QString table,QStringList column,QStringList value)
{
    QString strF;
    QSqlQuery query;
    int i;

    strF = "INSERT INTO "+table+" ( ";
    for(i=0;i<column.size();i++){
        strF = strF + column[i];
        if(i<column.size()-1)
            strF = strF + ", ";
    }

    strF = strF + ") VALUES (";

    for(i=0;i<column.size();i++){
        strF = strF + "'" + value[i] + "'";
        if(i<column.size()-1)
            strF = strF + ", ";
    }

    strF = strF + ");";

    if (!query.exec(strF)){
        qDebug() << "3c1f9d13f59a784144a975dd9f5f1fc4" << query.lastError() << strF;
        LogOut.logout(query.lastError().text());
        return query.lastError().number();
    }else{
        return 0;
    }
}

void BD::UpdateTable(QString table, QString column, QString value, QString where1, QString where2  )
{
    QString str;
    QSqlQuery query;
    QVariant t;
    t = SelectFromTable("SELECT "+column+" FROM "+table+" WHERE "+where1+" = '"+where2+"'");
    if (t.toString() != value ){
        str = "UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'";
        str = str.arg(table)
                    .arg(column)
                    .arg(value)
                    .arg(where1)
                    .arg(where2);
        if (!query.exec(str))
        {
            qDebug()<<"bf206b5a653a24ca32249bfd24515d4e" <<"Eror  "<<query.lastError()<<"\n"<<str;
            LogOut.logout(query.lastError().text());
        }
    }
}

//======================================================================
// Квартиры
int BD::is_idappart(int id_home, int id_org, int num)
{
    int out = -1;
    QString str, strF;
    str = "SELECT id_apartament FROM apartament WHERE id_homes='%1' AND id_organiz='%2' AND number='%3'";
    strF = str.arg(id_home)
           .arg(id_org)
           .arg(num);
    QVariant t = SelectFromTable(strF);
    if (!t.isNull()){
        out = t.toInt();
    }
    return out;
}

void BD::UpdateApartament(QStringList column, QStringList value, int idapart)
{
    for(int i=0; i<column.count();i++ ){
        UpdateTable("apartament",column[i],value[i],"id_apartament", QString::number(idapart));
    }
}

void BD::UpdateMenInApartament(QStringList column, QStringList value, int idapart, int year, int month)
{
    QString str;
    int out = -1;
    str = "SELECT COUNT(*) FROM men_in_apartament WHERE id_apartament = %1 AND year = %2 AND month = %3";
    str = str.arg(idapart)
            .arg(year)
            .arg(month);
    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        out = t.toInt();
    }
    qDebug() << "3163221858b740c726032cd15808cfdb" <<out;
    if (out == 1){
        for(int i=0; i<column.count();i++ ){
            UpdateTable("men_in_apartament",column[i],value[i],"id_apartament", QString::number(idapart));
        }
    }else if(out == 0){
        column << "id_apartament" << "year" << "month";
        value << QString::number(idapart) << QString::number(year) << QString::number(month);
        add("men_in_apartament",column,value);
    }
}

QString BD::is_FIO(int id_app)
{
    QString str;
    QSqlQuery query;

    str = "SELECT surname, name,patronymic  FROM apartament WHERE id_apartament = %1";
    str = str.arg(id_app);

    if (query.exec(str)){
        if (query.next()){
            return query.value(0).toString()+"  "+query.value(1).toString()+"  "+query.value(2).toString();
        }else{
            qDebug()<< "5c338ca2e2cf8d0530fd61f407f2d214" <<"not record" << str;
        }
    } else{
        qDebug()<< "e13253d437b3561bd7e8a82e1e675c55" <<query.lastError();
        LogOut.logout(query.lastError().text());
    }
    return "";
}

QStringList BD::sum_app(int id_org, int id_home)
{
    QString str;

    str = "SELECT number FROM apartament WHERE id_homes=%1 AND id_organiz=%2";
    str = str.arg(id_home)
            .arg(id_org);

    return strL_from_query(str);
}

QList<int> BD::is_ListIdApartament(int id_org, int id_home)
{
    QString str;
    QList<int> out ;
    QSqlQuery query;

    str = "SELECT id_apartament FROM apartament WHERE id_homes=%1 AND id_organiz=%2";
    str = str.arg(id_home)
            .arg(id_org);
    if (query.exec(str)){
      while (query.next()){
          out <<  query.value(0).toInt();
      }
    } else{
        qDebug()<< "709737893f541e722add6cf123fad78e" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    return out;
}

double BD::is_TotalArea(int id_app)
{
    QString str;

    str = "SELECT total_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    return qVariant_from_query(str).toDouble();
}

double BD::is_LivedArea(int id_app)
{
    QString str;

    str = "SELECT inhabed_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

   return qVariant_from_query(str).toDouble();
}

double BD::is_Lodjia(int id_app)
{
    QString str;

    str = "SELECT loggia FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    return qVariant_from_query(str).toDouble();
}

double BD::is_Balkon(int id_app)
{
    QString str;

    str = "SELECT balkon FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    return qVariant_from_query(str).toDouble();
}

int BD::is_NumberAppartament(int id_app)
{
    QString str;

    str = "SELECT number FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    return qVariant_from_query(str).toDouble();
}

int BD::is_LSh(int id_app)
{
    QString str;

    str = "SELECT personal_account FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    return qVariant_from_query(str).toInt();
}

//=====================================================================


QStringList BD::Sum_Schet(int id_apartament)
{
    QString str;

    str = "SELECT u.name FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    return strL_from_query(str);;
}

QStringList BD::Sum_Service(int id_apartament)
{
    QString str;

    str = "SELECT u.name FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND NOT u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    return strL_from_query(str);
}

QList<int> BD::is_ListIdServiceWithCounter(int id_apartament)
{
    QString str;
    QSqlQuery query;
    QList<int> out;

    str = "SELECT lau.id_usluga "
            "FROM usluga u, list_app_usluga lau "
            "WHERE u.id_usluga=lau.id_usluga AND u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    if (!query.exec(str)){
        qDebug() << "92df4946066ff2f748cbd0178e263cac" << "Eror in "<<str<<query.lastError();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toInt();
        }
    }
    return out;
}

QList<int> BD::is_ListIdServiceOutCounter(int id_apartament)
{
    QString str;
    QSqlQuery query;
    QList<int> out;

    str = "SELECT lau.id_usluga FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND NOT u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    if (!query.exec(str)){
        qDebug() << "657e96603000538d92a0d97f98694612"<< "Eror in "<<str<<query.lastError();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toInt();
        }
    }
    return out;
}

int BD::is_RealMen(int id_app, DateOfUnixFormat date)
{
    QString str;

    str = "SELECT real_men FROM men_in_apartament "
            " WHERE id_apartament=%1 AND date_men_in_apartament <= %2"
            " ORDER BY date_men_in_apartament";
    str = str.arg(id_app)
            .arg(date.Second());

    return qVariant_from_query(str).toInt();
}

int BD::is_RentMen(int id_app, DateOfUnixFormat date)
{
    QString str;

    str = "SELECT rent_men FROM men_in_apartament "
            " WHERE id_apartament=%1  AND date_men_in_apartament <= %2 "
            " ORDER BY date_men_in_apartament"
            ;
    str = str.arg(id_app)
            .arg(date.Second());

    return qVariant_from_query(str).toInt();
}
int BD::is_ReservMen(int id_app, DateOfUnixFormat date)
{
    QString str;

    str = "SELECT reserv_men FROM men_in_apartament "
            " WHERE id_apartament=%1 AND date_men_in_apartament <= %2"
            " ORDER BY date_men_in_apartament"
            ;
    str = str.arg(id_app)
            .arg(date.Second());

    return qVariant_from_query(str).toInt();
}



QList<int> BD::is_ApartamentService(int id_app)
{
    QList<int> out;   //список ид услуг относящихся к этой квартире

    QString str;
    QSqlQuery query;

    str = "SELECT id_usluga FROM list_app_usluga WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      while (query.next()){
          out <<  query.value(0).toInt();
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
    }

    return out;
}

QString BD::is_NameService(int id_service)
{
    QString out = "";
    QString str;
    QSqlQuery query;

    str = "SELECT name FROM usluga WHERE id_usluga=%1";
    str = str.arg(id_service);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toString();
      }else{
          qDebug()<<"not record" << str;
          out = "";
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = "";
    }

    return out;
}

QSqlQueryModel* BD::Model(QString table)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    if (table =="homes"){
        model->setQuery(QSqlQuery("SELECT name, id_homes FROM homes"));
    }else if(table == "organiz"){
        model->setQuery(QSqlQuery("SELECT name, id_organiz FROM organiz"));
    }else if(table == "usluga"){
        model->setQuery(QSqlQuery("SELECT name, id_usluga FROM usluga"));
    }
    return model;
}


QSqlQueryModel* BD::ModelHome(){
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_homes, name  FROM homes"));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Адрес"));
    return model;
}

QAbstractItemModel* BD::ModelApartament(int id_apartament) //
{     
    SqlQueryModelApartament *model = new SqlQueryModelApartament;
    model->SetIdApartament(id_apartament);

    connect(model,SIGNAL(sgn_RefreshModel(QAbstractTableModel*)),this,SLOT(sl_ModelApartamentHeaderData(QAbstractTableModel*)));
    connect(model,SIGNAL(sgn_EditIndex(int,QString,int)),this,SLOT(sl_EditApartament(int,QString,int)));
    QList<int> lst;
    lst << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9;
    model->setEditColumn(lst);

    model->setMyQuery(" SELECT "
                              " number, surname, name, patronymic, "
                              " total_area, inhabed_area,balkon, loggia,  personal_account  "
                              " FROM apartament "
                              " WHERE id_apartament = "+QString::number(id_apartament));

    sl_ModelApartamentHeaderData(model);

    TransposeProxyModel *trans = new TransposeProxyModel;
    trans->setSourceModel(model);
    return trans;

}

void BD::sl_ModelApartamentHeaderData(QAbstractTableModel *model)
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
}

void BD::sl_EditApartament(int col,QString val,int id_apart)
{
    QStringList lst_col;
    QStringList lst_val;

    qDebug() << col;

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
    UpdateApartament(lst_col,lst_val,id_apart);
}

QSqlQueryModel* BD::ModelApartament(int id_home, int id_org)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT number, id_apartament FROM apartament "
            " WHERE id_homes = "+QString::number(id_home)+" AND id_organiz = "+QString::number(id_org);
    model->setQuery(QSqlQuery(str));

    return model;
}

QSqlQueryModel* BD::ModelUslugiTabl(int id_apartament)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT lau.id_list_app_usluga, u.name FROM list_app_usluga lau, usluga u "
            " WHERE lau.id_apartament = "+QString::number(id_apartament)+" AND u.id_usluga=lau.id_usluga";
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Услуги"));

    return model;
}



QSqlQueryModel* BD::ModelPensioner(int id_home, int id_org)
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
//    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Сумма"));
    return model;
}
QSqlError BD::DeletePension(int id_apart)
{
    QString str;
    QSqlQuery query;
    QSqlError out;

    str = "DELETE FROM pensioner_living_alone WHERE id_apartament=%1";
    str = str.arg(id_apart);
    if (!query.exec(str)){
        out = query.lastError();
    }
    return out;
}

//--------------------------------------------------------------------------------------------------------

QSqlError BD::DeleteUslugaApartament(int id_list_apart_usluga)
{
    QString str;
    QSqlQuery query;
    QSqlError out;

    str = "DELETE FROM list_app_usluga WHERE id_list_app_usluga=%1";
    str = str.arg(id_list_apart_usluga);
    if (!query.exec(str)){
        out = query.lastError();
    }
    return out;
}

QSqlQueryModel* BD::ModelPokazanie(int id_apartament, int month, int year)
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
                add("pokazanie",column,values);
            }
        } else{
            qDebug()<<query2.lastError();
            LogOut.logout(query2.lastError().text());
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

SqlQueryEditModel* BD::ModelEditPokazanie(int id_apartament, int month, int year)
{
    SqlQueryEditModel *model = new SqlQueryEditModel;
    connect(model,SIGNAL(sgn_EditPokazanie(int,QString)),this,SLOT(sl_EditPokazanie(int,QString)));
    connect(model,SIGNAL(sgn_RefreshModel(QAbstractTableModel*)),this,SLOT(sl_ModelPokazanieHeaderData(QAbstractTableModel*)));
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
    sl_ModelPokazanieHeaderData(model);
    return model;
}

void BD::sl_ModelPokazanieHeaderData(QAbstractTableModel *t)
{
    t->setHeaderData(0, Qt::Horizontal, QObject::trUtf8("№"));
    t->setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Счётчик"));
    t->setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Показазия текущие"));
}

int BD::NewApatament(int id_org, int id_home, int num_apart)
{
    QStringList column, value;

    column<<"id_homes"<<"id_organiz"<<"number";

    value.append(QString::number(id_home));
    value.append(QString::number(id_org));
    value.append(QString::number(num_apart));
    if (add("apartament",column,value) != 0){
        return -1;
    }
    return 0;
}

int BD::is_TypeUsluga(int id_usluga)
{
    QString str;

    str = "SELECT type_usluga FROM usluga WHERE id_usluga=%1";
    str = str.arg(id_usluga);

    return qVariant_from_query(str).toInt();
}

int BD::is_idListAppUsluga(int id_apartament, int id_usluga)
{
    QString str;

    str = "SELECT id_list_app_usluga FROM list_app_usluga WHERE id_usluga=%1 AND id_apartament=%2";
    str = str.arg(id_usluga)
            .arg(id_apartament);

   return qVariant_from_query(str).toInt();
}

void BD::sl_EditPokazanie(int id_pok, QString value)
{
    QSqlQuery query;
    query.prepare("UPDATE pokazanie set pokazanie_end = ? WHERE id_pokazanie = ?");
    query.addBindValue(value);
    query.addBindValue(id_pok);
    if (!query.exec())
    {
        qDebug()<<query.lastError()<<"\n";
        LogOut.logout(query.lastError().text());
    }
    SumCount(id_pok); //канализация
    new_pokazanie(id_pok,value); //новое показание на след месяц
}
void BD::new_pokazanie(int id_apartament, int month, int year)
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
            new_pokazanie(query.value(0).toInt(),query.value(1).toString());
        }
    }else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
    }
}

int BD::new_pokazanie(int id_pok_old, QString value_home)
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
            qDebug()<< "not record " << str;
        }
    } else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
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
    QVariant t = SelectFromTable(str);
    if(t.isNull()){//если нет записей
        //добавим новое показание на след месяц
        column.clear();
        column<<"id_list_app_usluga"<<"date_pokazanie"<<"pokazanie_home"<<"pokazanie_end";
        value.append(QString::number(id_ListApart));
        value.append(QString::number(date.Second()));
        value.append(value_home);
        value.append(QString::number(0));
        if ((id_new = add("pokazanie",column,value)) != 0){
            return -1;
        }
    }else{//иначе
        //Обновим запись
        UpdateTable("pokazanie","pokazanie_home",value_home,"id_pokazanie",t.toString());
        return t.toInt();
    }
    return id_new;
}

void BD::CreditedOfService(int id_apartament, DateOfUnixFormat date)  //расчёт для квартиры
{
    QList<int> ListService;
    QString str;
    QSqlQuery query;
    ListService = is_ApartamentService(id_apartament);                  // список ид услуг по квартире

    for(int i=0;i<ListService.size();i++){
        int id_list_ap_usl = -1;
        id_list_ap_usl = is_idListAppUsluga(id_apartament, ListService[i]);

        str = "SELECT id_credited FROM credited WHERE id_list_app_usluga=%1 AND date_credited=%2 ";
        str = str.arg(id_list_ap_usl)
                .arg(date.Second());

        // начисление по услугам
        if (query.exec(str)){
            QVariant cred = CreditedOfApartament(id_list_ap_usl, date);
            if (query.next()){
                QVariant credR = SelectFromTable("SELECT credited_of_service FROM credited "
                                "WHERE id_credited = "+query.value(0).toString());
                if(cred.toString() != credR.toString()){
                    UpdateTable("credited","credited_of_service",
                                cred.toString(),
                                "id_credited", query.value(0).toString()
                    );
                }
            }else{
                QStringList column,value;
                column<<"id_list_app_usluga"<<"date_credited"<<"credited_of_service";
                value<<QString::number(id_list_ap_usl)<<
                    QString::number(date.Second())<<
                    CreditedOfApartament(id_list_ap_usl,date).toString();
                add("credited",column,value);
            }
        } else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
        }

    }
    CreditedForApartament(id_apartament,date);  //расчёт общей оплаты и задолжности
}

int BD::is_Pokazanie(int id_list_app_usluga, QDate date)
{
    QString str;
    int out = -1;
    QDate next_date;
    next_date = date.addMonths(1);
    DateOfUnixFormat unix_date(next_date);

    str = "SELECT pokazanie_home FROM pokazanie WHERE id_list_app_usluga=%1 AND date_pokazanie=%2 ";
    str = str.arg(id_list_app_usluga)
            .arg(unix_date.Second());
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toInt();   
    }

    str = " SELECT u.id_usluga FROM usluga u, list_app_usluga lau "
            " WHERE lau.id_list_app_usluga=%1 AND lau.id_usluga = u.id_usluga";
    str = str.arg(id_list_app_usluga);
    t = SelectFromTable(str);
    if(!t.isNull()){
        if (t.toInt() == 3){
            out = 0;
        }
    }

    return out;
}

QVariant BD::CreditedOfApartament(int id_list_app_usluga, DateOfUnixFormat date) // начисление услуга-квартрира-дата
{
    QString str;
    QSqlQuery query;
    int type_usluga, id_usluga;
    double tarif = 0;
    QVariant out;

    //получим тип услуги ид услуги и тариф
    str = "SELECT u.type_usluga, u.id_usluga FROM usluga u, list_app_usluga lau "
            "WHERE u.id_usluga=lau.id_usluga AND lau.id_list_app_usluga=%1";
    str = str.arg(id_list_app_usluga);

    if (query.exec(str)){
      if (query.next()){
          type_usluga = query.value(0).toInt();
          id_usluga = query.value(1).toInt();
          table_tariff tbl_tariff;
          tarif = tbl_tariff.is_Tariff(id_usluga,date);// Получаем тариф
      }else{
          qDebug()<<"not record" << str;
          return -1;
      }
    } else{
        qDebug()<<"CreditedOfApartament"<<query.lastError();
        LogOut.logout(query.lastError().text());
        return -1;
    }

    if(type_usluga == 1){ //счётчики
        out = PaymentCounters(id_list_app_usluga, date);
    }
    if(type_usluga == 2){ //на кв метр
        QString str2;
        QSqlQuery query2;
        str2 = "SELECT total_area FROM apartament a, list_app_usluga lau "
                "WHERE lau.id_apartament=a.id_apartament AND lau.id_list_app_usluga=%1";
        str2 = str2.arg(id_list_app_usluga);
        if (query2.exec(str2)){
          if (query2.next()){
              out = (QVariant)(tarif * query2.value(0).toDouble());
          }else{
              qDebug()<<"not record" << str;
              return -1;
          }
        } else{
            qDebug()<<"CreditedOfApartament"<<query.lastError();
            LogOut.logout(query.lastError().text());
            return -1;
        }
    }
    if(type_usluga == 3){ //на человека
        QString str2;
        QSqlQuery query2;
        str2 = "SELECT real_men FROM men_in_apartament a, list_app_usluga lau "
                "WHERE lau.id_apartament=a.id_apartament AND lau.id_list_app_usluga=%1";
        str2 = str2.arg(id_list_app_usluga);
        if (query2.exec(str2)){
          if (query2.next()){
              out = tarif * query2.value(0).toDouble();
          }else{
              qDebug()<<"not record" << str;
              return -1;
          }
        } else{
            qDebug()<<"CreditedOfApartament on men"<<query.lastError();
            LogOut.logout(query.lastError().text());
            return -1;
        }
    }
    if(type_usluga == 4){ //на квартиру
        out = tarif;
    }
    return out;
}

double BD::CreditedForReport(int id_apartament, int id_usluga, DateOfUnixFormat date)
{
    QString str;
    double out = -1;

    str = "SELECT credited_of_service FROM credited c, list_app_usluga lau "
            "WHERE date_credited=%1 AND lau.id_list_app_usluga=c.id_list_app_usluga "
            "AND lau.id_apartament=%2 AND lau.id_usluga=%3 ";
    str = str.arg(date.Second())
            .arg(id_apartament)
            .arg(id_usluga);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toDouble();
    }
    return out;
}

void BD::CreditedForApartament(int id_apart, DateOfUnixFormat date)
{
    QString str;
    QSqlQuery query;
    QVariant cred_with_count, cred_out_count;


    // К оплате по услугам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.date_credited=%1 "
            "AND c.id_list_app_usluga=lau.id_list_app_usluga "
            "AND lau.id_usluga=u.id_usluga "
            "AND NOT u.type_usluga='1' "
            "AND lau.id_apartament=%2 ";
    str = str.arg(date.Second())
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        cred_out_count = t;
    }

    // К оплате по счётчикам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.date_credited=%1 "
                "AND c.id_list_app_usluga=lau.id_list_app_usluga "
                "AND lau.id_usluga=u.id_usluga "
                "AND u.type_usluga=1 "
                "AND lau.id_apartament=%2";
    str = str.arg(date.Second())
            .arg(id_apart);
    t = SelectFromTable(str);
    if(!t.isNull()){
        cred_with_count = t;
    }
    str = "SELECT id_credited_of_apartament FROM credited_of_apartament "
            "WHERE date_credited_of_apartament=%1 AND id_apartament=%2 ";
    str = str.arg(date.Second())
            .arg(id_apart);
    if (query.exec(str)){
        if (query.next()){
            UpdateTable("credited_of_apartament","credited_with_counter",cred_with_count.toString(),
                        "id_credited_of_apartament",query.value(0).toString());
            UpdateTable("credited_of_apartament","credited_out_counter",cred_out_count.toString(),
                        "id_credited_of_apartament",query.value(0).toString());
        }else{
            QStringList column, value;
            column<<"id_apartament"<<"date_credited_of_apartament"<<"credited_with_counter"<<"credited_out_counter";
            value<<QString::number(id_apart)<<QString::number(date.Second())
                <<cred_with_count.toString()<<cred_out_count.toString();
            add("credited_of_apartament",column,value);
        }
    }else{
        qDebug()<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
    }
    PaymentOfDebt(id_apart,date.year(),date.month());// Расчёт долга
}

double BD::AmountToPay(int id_apart, qint64 u_date)
{
    QString str;
    double out = 0.0, debt = 0.0;

    str="SELECT debt FROM debt WHERE  date_debt=%1 AND id_apartament=%2";

    str = str.arg(u_date)
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        debt = t.toDouble();
    }

    out = debt + AmountForServices(id_apart,u_date);
//    qDebug() << out << u_date << debt;
    return out;
}

void BD::PaymentOfDebt(int id_apart, int year, int month/*DateOfUnixFormat date*/)
{
    QString str;
    QSqlQuery query;
    double debt = 0.0, payment = 0.0;
    DateOfUnixFormat  date(year,month,25);

    // --------- К оплате в прошлом месяце
    debt = debt + AmountToPay(id_apart,date.Second_first_day(-1));
//    qDebug() <<"debt1 " << debt;
    //---------- Долг за счётчики в этом месяце
    str="SELECT credited_with_counter FROM credited_of_apartament WHERE  date_credited_of_apartament=%1 AND id_apartament=%2";
    str = str.arg(date.Second())
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        debt = debt + t.toDouble();
    }
//    qDebug() << "t4" << payment << debt;
    //---------- Оплата после 25 числа прошлого месяца по 25 число этого месяца
    str = "SELECT payment FROM payment "
            "WHERE payment_date >= %1 AND payment_date <= %2 AND id_apartament = %3";
    str = str.arg(date.Second(-1))
            .arg(date.Second())
            .arg(id_apart);

    if (query.exec(str)){
        while (query.next()){
            payment = payment + query.value(0).toDouble();
//            qDebug() << query.value(0);
        }
    }else{
        LogOut.logout(query.lastError().text());
    }
    debt = debt - payment;
//    qDebug() << "t3" << payment << debt;
    //-----обновление/добавление долга
    str = "SELECT id_debt, debt FROM debt WHERE date_debt=%1 AND id_apartament=%2";
    str = str.arg(date.Second_first_day())
            .arg(id_apart);
    if (query.exec(str)){
//        qDebug() << "z1";
        if (query.next()){
            if(fabs(debt - query.value(1).toDouble()) <0.01){
                return;
            }else{
                UpdateTable("debt","debt",QString::number(debt,'f',2),"id_debt",query.value(0).toString());
            }
//            qDebug() << "z2";
        }else{
            QStringList column,value;
            column << "date_debt" << "id_apartament" << "debt";
            value << QString::number(date.Second())
                  << QString::number(id_apart) << QString::number(debt,'f',2);
            add("debt",column,value);

//            qDebug() << "z3";
        }
    }else{
        qDebug()<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
        return;
    }
}
//-------------------------------------------------------------------------------------------
QString BD::is_Debt(int id_apart, DateOfUnixFormat date)
{
    QString str;
    double debt = 0.0;
    QString out= "" ;

    str="SELECT debt FROM debt WHERE  date_debt=%1 AND id_apartament=%3";
    str = str.arg(date.Second())
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        debt = t.toDouble();
    }

    if (debt>0){
        out = QObject::trUtf8("Ваш долг составляет:  ") + QString::number(debt,'f',2) + QObject::trUtf8(" p. ");
    }else if(debt<0){
        out = QObject::trUtf8("Ваша переплата составляет:  ") + QString::number(debt,'f',2) + QObject::trUtf8(" p. ");
    }
    return out;
}


double BD::AmountForServices(int id_apart, qint64 u_date)
{
    QString str;
    double out = 0;

    str="SELECT credited_out_counter FROM credited_of_apartament WHERE date_credited_of_apartament='%1' AND id_apartament=%2";
    str = str.arg(u_date)
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toDouble();
    }

    return out;
}
bool BD::isElectroUsluga(int id_usluga)
{
    if(id_usluga == 4 || id_usluga == 6 || id_usluga == 7){
        return true;
    }

    return false;
}

bool BD::is_pensioner_living_alone(int id_apartament)
{
    QString str;
    int count = -1;

    str = "SELECT COUNT(*) FROM pensioner_living_alone WHERE id_apartament = %1";
    str = str.arg(id_apartament);

    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        count = t.toInt();
    }
    if (count>0){
        return true;
    }

    return false;

}

double BD::PaymentCounters(int id_list_app_usluga, DateOfUnixFormat date)  //расчёт оплаты за счётчик
{
    QString str;
    QSqlQuery query;
    int id_usluga, id_apartament;
    double tarif,tarif2,norma, out = -1;
    DateOfUnixFormat previous_date(date.year(),date.month(),date.day());
    previous_date = date.addMonths(-1);


    //Получим ид квартиры и ид услуги
    str = "SELECT id_usluga, id_apartament FROM list_app_usluga WHERE id_list_app_usluga=%1";
    str = str.arg(id_list_app_usluga);
    if (query.exec(str)){
        if (query.next()){
            id_usluga = query.value(0).toInt();
            id_apartament = query.value(1).toInt();
        }else{
            qDebug()<<"not record" << str;
            return -1;
        }
    } else{
        qDebug()<<"CreditedOfApartament"<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
        return -1;
    }

    //получим тариф
    str = "SELECT t.tariff, t.tariff2, t.norm FROM tariff t, list_app_usluga lau "
            "WHERE lau.id_usluga=t.id_usluga AND lau.id_list_app_usluga=%1 AND tariff_date=%2 ";
    str = str.arg(id_list_app_usluga)
            .arg(previous_date.Second());
    if (query.exec(str)){
      if (query.next()){
            tarif = query.value(0).toDouble();
            tarif2 = query.value(1).toDouble();
            norma = query.value(2).toDouble();
      }else{
          return -1;
      }
    } else{
        qDebug()<<"50989f8dadb00b65e3396658407c7197 "<<query.lastError();
        LogOut.logout(query.lastError().text());
        return -1;
    }

    if(is_TypeUsluga(id_usluga)==1){
        str = "SELECT pokazanie_end, pokazanie_home FROM pokazanie "
                "WHERE id_list_app_usluga=%1 "
                "AND date_pokazanie=%2 ";
        str = str.arg(id_list_app_usluga)
                .arg(date.Second());
        if (query.exec(str)){
            if (query.next()){
                int count = query.value(0).toDouble() - query.value(1).toDouble();
                if(isElectroUsluga(id_usluga)){// Электричество
                    if(count <= norma*is_RealMen(id_apartament,date)|| norma==0 || is_pensioner_living_alone(id_apartament)){
                        out = tarif * count;
                    }else if (count > norma*is_RealMen(id_apartament,date)&& norma!=0){
                        out = norma * is_RealMen(id_apartament,date) * (tarif - tarif2) + count * tarif2;
                    }
                }else{
                    out = tarif * count;
                }

            }else{
                return -1;
            }
        }else{
            qDebug()<<"91748951796616cc5b01273b7a07c8f7"<<query.lastError();
            LogOut.logout(query.lastError().text());
            return -1;
        }
    }
    return out;
}

void BD::SumCount(int id_pokazanie, bool New/* = false*/) //Расчёт показаний канализации
{
    QString str;
    QSqlQuery query;
    int id_app, value = 0, id_sumpok;
    qint64 Unix_date;


    str = " SELECT date_pokazanie, id_apartament"            //Дата и квартира
          " FROM pokazanie p, list_app_usluga lau "
          " WHERE id_pokazanie = %1 "
            " AND lau.id_list_app_usluga = p.id_list_app_usluga ";
    str = str.arg(id_pokazanie);

    if (query.exec(str)){
        if (query.next()){
            Unix_date = query.value(0).toULongLong();
            id_app = query.value(1).toInt();
        }else{
            return;
        }
    }else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    }
    if (!New){//Если нет метки нового счётчика
        str = " SELECT pokazanie_end ";                              //показания за дату у квартиры
    }else{
        str = " SELECT pokazanie_home ";
    }
    str +=  " FROM pokazanie p, list_app_usluga lau "
            " WHERE date_pokazanie = %1 "
            " AND id_apartament = %2 "
            " AND lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND (id_usluga=1 OR id_usluga=2)";                // Воды и ГВС

    str = str.arg(Unix_date)
                .arg(id_app);

    if (query.exec(str)){
        while (query.next()){
            value = value + query.value(0).toInt();             //Всё суммируем
        }
    }else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    }

    if (!New){
        str = " SELECT id_pokazanie, pokazanie_end ";
    } else {
        str = " SELECT id_pokazanie, pokazanie_home ";
    }
    str +=  " FROM pokazanie p, list_app_usluga lau "
            " WHERE lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND date_pokazanie = %1 "
            " AND id_apartament = %2"
            " AND id_usluga = 3";                           //ид счётчика канализации

        str = str.arg(Unix_date)
                .arg(id_app);

    if (query.exec(str)){
        if (query.next()){
            id_sumpok = query.value(0).toInt();
            if (query.value(1).toInt() != value){
                if(!New){
                    UpdateTable("pokazanie","pokazanie_end",
                                QString::number(value),"id_pokazanie",QString::number(id_sumpok));
                } else {
                    UpdateTable("pokazanie","pokazanie_home",
                                QString::number(value),"id_pokazanie",QString::number(id_sumpok));
                }
            }
            if (!New){
                new_pokazanie(id_sumpok,QString::number(value)); //показание на начало след месяца
            }
        }else{           
            return;
        }
    }else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    } 
}

void BD::UpdateOrganization(QString name, QString bank, QString acc, QString inn, int idorg)
{
    UpdateTable("organiz","name",name,"id_organiz",QString::number(idorg));
    UpdateTable("organiz","bank",bank,"id_organiz",QString::number(idorg));
    UpdateTable("organiz","sett_account",acc,"id_organiz",QString::number(idorg));
    UpdateTable("organiz","inn",inn,"id_organiz",QString::number(idorg));
}

QString BD::DeleteOrg(int id_org)
{
    QString str;
    QSqlQuery query;
    QString out = "";

    str = "DELETE FROM organiz WHERE id_organiz=%1";
    str = str.arg(id_org);
    if (!query.exec(str)){
        out = query.lastError().text();
    }
    return out;
}

QString BD::DeleteHome(int id_home)
{
    QString str;
    QSqlQuery query;
    QString out = "";

    str = "DELETE FROM homes WHERE id_homes=%1";
    str = str.arg(id_home);
    if (!query.exec(str)){
        out = query.lastError().text();
    }
    return out;
}

QString BD::is_DatabaseVersoin()
{
    QString str;
    QSqlQuery query;
    QString res = "";

    str = "SELECT version FROM version";
    if (query.exec(str)) {
        if (query.next()){
            res = trUtf8("Версия БД - ")+query.value(0).toString();
        }else{
            res = "Version not found";
            LogOut.logout("Version not found");
        }
    }else{
        res = query.lastError().text();
        LogOut.logout(query.lastError().text());
    }
    return res;
}

QString BD::is_NameCounter(int id_counter)
{
    QString str;
    QString out = "";

    str = " SELECT u.name FROM pokazanie p, list_app_usluga lau, usluga u "
            " WHERE  p.id_pokazanie = %1"
            " AND lau.id_usluga = u.id_usluga"
            " AND p.id_list_app_usluga = lau.id_list_app_usluga";
    str = str.arg(id_counter);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toString();
    }

    return out;
}

void BD::UpdatePokazanieHome(int id_pokazanie, int new_pokazanie)
{
    UpdateTable("pokazanie","pokazanie_home",QString::number(new_pokazanie),"id_pokazanie",QString::number(id_pokazanie));
    SumCount(id_pokazanie,true);
}

int BD::is_IdPokazanie(int id_list_app_usluga, DateOfUnixFormat date)
{
    QString str;
    int out = -1;

    str = "SELECT id_pokazanie FROM pokazanie WHERE id_list_app_usluga = %1 AND date_pokazanie = %2";
    str = str.arg(id_list_app_usluga)
            .arg(date.Second(1));

    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toInt();
    }
    return out;
}

void BD::UpdateHome(int id_home, QString home)
{
    UpdateTable("homes","name",home,"id_homes",QString::number(id_home));
}

QSqlQueryModel* BD::ModelSettings()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT name_setting,value_setting FROM settings"));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Название"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Значение"));
    return model;
}

QSqlError BD::DeleteSetting(QString name_setting)
{
    QString str;
    QSqlQuery query;
    QSqlError out;

    str = "DELETE FROM settings WHERE name_setting = '%1'";
    str = str.arg(name_setting);
    if (!query.exec(str)){
        out = query.lastError();
    }
    return out;
}

QString BD::isValueSetting(QString NameSetting)
{
    QString out = "";
    QString str;
    QSqlQuery query;

    str = "SELECT value_setting FROM settings WHERE name_setting = '%1'";
    str = str.arg(NameSetting);

    if (query.exec(str)){
        if (query.next()){
            out = query.value(0).toString();
        }else{
            QStringList column, value;
            column << "name_setting" << "value_setting";
            value << NameSetting << "";
            add("settings", column, value);
        }
    }

    return  out;
}

int BD::is_IdHome(QString Home_name)
{
    int out = -1;
    QString str;

    str = "SELECT id_homes FROM homes WHERE name = '%1'";
    str = str.arg(Home_name);

    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toInt();
    }

    return out;
}



QStringList BD::strL_from_query(QString str)
{
    QSqlQuery query;
    QStringList out;

    if (!query.exec(str)){
        qDebug() << "98c4ea14e87b9c76588b823d00e3d957" << query.lastError().text();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toString();
        }
    }
    return out;
}

QVariant BD::qVariant_from_query(QString str)
{
    QVariant out;
    QSqlQuery query;

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0);
      }else{
          qDebug() << "not record" << str;
          out = -1;
      }
    } else{
        qDebug() << "21a3cce63f41f666b02173c48ef61963" << query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}
