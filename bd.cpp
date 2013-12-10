#include "bd.h"
#include "table_tariff.h"


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
//------------------------------------------------------------------------------------------------------
void BD::Create()
{
    if (!RunScript("./update_db/baseline.sql")){
        qDebug() << "Error run " << "./update_db/baseline.sql";
    }

}
//------------------------------------------------------------------------------------------------------
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
                if((list_with_sql.at(i).right(3).toInt() > subversion.toInt())){
                    list_update << dir_with_sql.absoluteFilePath(list_with_sql.at(i));
                }
            }
        }
    }
<<<<<<< HEAD
=======

    for (int i=0;i<list_update.count();i++){
        if (!RunScript(list_update.at(i))){
            qDebug() << "Error run " << list_update.at(i);
        }
    }
>>>>>>> db 05.12 work

}
//-------------------------------------------------------------------------------------------------------

QVariant BD::SelectFromTable(QString str)
{
    QSqlQuery query;
    QVariant out = -1;

    if (query.exec(str)){
        if (query.next()){
            out = query.value(0);
        }else{
            qDebug()<<"In query "<<str<<" not found line ";
            LogOut.logout("In query " + str + " not found line ");
        }
    } else{
        qDebug() << query.lastError() << str;
        LogOut.logout(query.lastError().text());
    }
    return out;
}
//--------------------------------------------------------------------------------------------------------
int BD::add(QString table, QString column, QString value)
{
    QStringList C_sl, V_sl;
    C_sl<<column;
    V_sl<<value;
    return add(table,C_sl,V_sl);
}
//--------------------------------------------------------------------------------------------------------
int BD::add(QString table,QStringList column,QStringList value)
{
    qDebug()<<"Add Line in "<< table;
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

    qDebug()<< strF;

    if (!query.exec(strF)){
        qDebug() << "Unable to do " << strF <<query.lastError();
        LogOut.logout(query.lastError().text());
        return -1;
    }else{
        return 0;
    }
}
//--------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------
void BD::UpdateTable(QString table, QString column, QString value, QString where1, QString where2  )
{
    QString str;
    QSqlQuery query;
    QVariant t;
    t = SelectFromTable("SELECT "+column+" FROM "+table+" WHERE "+where1+" = "+where2);
    if (t.toString() != value ){
        str = "UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'";
        str = str.arg(table)
                    .arg(column)
                    .arg(value)
                    .arg(where1)
                    .arg(where2);
        if (!query.exec(str))
        {
            qDebug()<<"Eror  "<<query.lastError()<<"\n"<<str;
            LogOut.logout(query.lastError().text());
        }
    }
}

//----------------------------------------------------------------------------------------------------
void BD::UpdateApartament(QStringList column, QStringList value, int idapart)
{
    for(int i=0; i<column.count();i++ ){
        UpdateTable("apartament",column[i],value[i],"id_apartament", QString::number(idapart));
    }
}
//----------------------------------------------------------------------------------------------------
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
    qDebug() << out;
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

//-------------------------------------------------------------------------------------------------------
QString BD::is_nameOrg(int id)
{
    QString str;
    QSqlQuery query;

    str = "SELECT name, bank, sett_account FROM organiz WHERE id_organiz = %1";
    str = str.arg(id);

    if (query.exec(str)){
      if (query.next()){
          return query.value(0).toString()+"  "+query.value(1).toString()+"  "+query.value(2).toString();
      }else{
          qDebug()<<"not record" << str;
      }

    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
    }
    return "";
}

QString BD::is_SmallnameOrg(int id)
{
    QString str;
    QString out;

    str = "SELECT name FROM organiz WHERE id_organiz = %1";
    str = str.arg(id);

    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        out = t.toString();
    }
    return out;
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
            qDebug()<<"not record" << str;
        }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
    }
return "";
}
QString BD::is_nameHome(int id)
{
    QString str, out="";
    str = "SELECT name FROM homes WHERE id_homes = %1";
    str = str.arg(id);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toString();
    }
    return out;
}

QStringList BD::sum_app(int id_org, int id_home)
{
    QString str;
    QSqlQuery query;
    QStringList out;

    str = "SELECT number FROM apartament WHERE id_homes=%1 AND id_organiz=%2";
    str = str.arg(id_home)
            .arg(id_org);

    if (!query.exec(str)){
        qDebug() << "Eror in "<<str;
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toString();
        }
    }
    return out;
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
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
    }

    return out;
}

QStringList BD::Sum_Schet(int id_apartament)
{
    QString str;
    QSqlQuery query;
    QStringList out;

    str = "SELECT u.name FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    if (!query.exec(str)){
        qDebug() << "Eror in "<<str<<query.lastError();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toString();
        }
    }
    return out;
}

QStringList BD::Sum_Service(int id_apartament)
{
    QString str;
    QSqlQuery query;
    QStringList out;

    str = "SELECT u.name FROM usluga u, list_app_usluga lau "
            " WHERE u.id_usluga=lau.id_usluga AND NOT u.type_usluga=1 AND lau.id_apartament=%1";
    str = str.arg(id_apartament);

    if (!query.exec(str)){
        qDebug() << "Eror in "<<str<<query.lastError();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toString();
        }
    }
    return out;
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
        qDebug() << "Eror in "<<str<<query.lastError();
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
        qDebug() << "Eror in "<<str<<query.lastError();
        LogOut.logout(query.lastError().text());
    } else {
        while (query.next()){
            out << query.value(0).toInt();
        }
    }
    return out;
}

int BD::is_LSh(int id_app)
{
    int out;
    QString str;
    QSqlQuery query;

    str = "SELECT personal_account FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toInt();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

int BD::is_RealMen(int id_app, int year, int month)
{
    int out;
    QString str;
    QSqlQuery query;

    str = "SELECT real_men FROM men_in_apartament "
            " WHERE id_apartament=%1 AND ((year = %2 AND month <= %3) OR (year < %2))"
            " ORDER BY year DESC, month DESC";
    str = str.arg(id_app)
            .arg(year)
            .arg(month);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toInt();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

int BD::is_RentMen(int id_app, int year, int month)
{

    int out;
    QString str;
    QSqlQuery query;

    str = "SELECT rent_men FROM men_in_apartament "
            " WHERE id_apartament=%1  AND ((year = %2 AND month <= %3) OR (year < %2)) "
            " ORDER BY year DESC, month DESC";
    str = str.arg(id_app)
            .arg(year)
            .arg(month);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toInt();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError()<< str;
        LogOut.logout(query.lastError().text());
        out = -1;
    }
//    qDebug()<<"test45"<<out;
    return out;
}
int BD::is_ReservMen(int id_app, int year, int month)
{
    int out;
    QString str;
    QSqlQuery query;
//    qDebug()<<"test5";
    str = "SELECT reserv_men FROM men_in_apartament "
            " WHERE id_apartament=%1 AND ((year = %2 AND month <= %3) OR (year < %2))"
            " ORDER BY year DESC, month DESC";
    str = str.arg(id_app)
            .arg(year)
            .arg(month);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toInt();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

double BD::is_TotalArea(int id_app)
{
    double out;
    QString str;
    QSqlQuery query;

    str = "SELECT total_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toDouble();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

double BD::is_LivedArea(int id_app)
{
    double out;
    QString str;
    QSqlQuery query;

    str = "SELECT inhabed_area FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toDouble();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

double BD::is_Lodjia(int id_app)
{
    double out;
    QString str;
    QSqlQuery query;

    str = "SELECT loggia FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toDouble();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

double BD::is_Balkon(int id_app)
{
    double out;
    QString str;
    QSqlQuery query;

    str = "SELECT balkon FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toDouble();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
}

int BD::is_NumberAppartament(int id_app)
{
    int out;
    QString str;
    QSqlQuery query;

    str = "SELECT number FROM apartament WHERE id_apartament=%1";
    str = str.arg(id_app);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toDouble();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }

    return out;
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

QStringList BD::is_nameOrg()
{
    QStringList out;

    QString str;
    QSqlQuery query;


    str = "SELECT name, bank, sett_account FROM organiz";

    if (query.exec(str)){
      while (query.next()){
          out << (query.value(0).toString()+"  "+query.value(1).toString()+"  "+query.value(2).toString());
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
    }

    return out;
}

QStringList BD::is_nameHome()
{
    QStringList out;

    QString str;
    QSqlQuery query;


    str = "SELECT name FROM homes";

    if (query.exec(str)){
      while (query.next()){
          out << query.value(0).toString();
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
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

QSqlQueryModel* BD::ModelOrganiz()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_organiz, name, bank,sett_account  FROM organiz"));


    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Организация"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Банк"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Лицевой счёт"));
    return model;
}

QSqlQueryModel* BD::ModelHome(){
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_homes, name  FROM homes"));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Адрес"));
    return model;
}

QSqlQueryModel* BD::ModelApartament(int id_apartament) //
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT * FROM apartament "
                              " WHERE id_apartament="+QString::number(id_apartament)));

    return model;
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



//QSqlQueryModel* BD::ModelBay(int id_apartament)
//{
//    QSqlQueryModel *model = new QSqlQueryModel;
//    QString str;
//    str = "SELECT  strftime('%d',bay_date,'unixepoch'), strftime('%m',bay_date,'unixepoch'),strftime('%Y',bay_date,'unixepoch'), bay FROM bay WHERE id_apartament = "+QString::number(id_apartament);
//    model->setQuery(QSqlQuery(str));
//    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("День"));
//    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Месяц"));
//    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Год"));
//    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Сумма"));

//    return model;
//}
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
    qDebug()<<model->lastError();
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
    str = "SELECT p.id_pokazanie, u.name, p.pokazanie_home, p.pokazanie_end, t.tarif, t.tarif2, t.norm "
            "FROM list_app_usluga lau, usluga u, tarif t, pokazanie p "
            "WHERE "
            "lau.id_usluga=u.id_usluga "
            "AND t.id_usluga=u.id_usluga "
            "AND u.type_usluga=1 "
            "AND lau.id_apartament="+QString::number(id_apartament)+" "
            "AND p.id_list_app_usluga=lau.id_list_app_usluga "
            "AND t.month_t=p.date_month "
            "AND p.date_month="+QString::number(month)+" "
            "AND p.date_year=t.year_t "
            "AND p.date_year="+QString::number(year)+" ";
    model->setQuery(QSqlQuery(str));

    if(model->rowCount()==0){//  если строки не найдены, скорее всего нет тарифов - сделаем без них
        QWidget *wgt = new QWidget;
//        QMessageBox::information(wgt,trUtf8("Предупреждение"),
//                             trUtf8("Возможно не установленны тарифы. \n "),QMessageBox::Ok);
        delete wgt;
        QString str2 = "SELECT p.id_pokazanie, u.name, p.pokazanie_home, p.pokazanie_end "
            "FROM list_app_usluga lau, usluga u, pokazanie p "
            "WHERE "
            "lau.id_usluga=u.id_usluga "
            "AND u.type_usluga=1 "
            "AND lau.id_apartament="+QString::number(id_apartament)+" "
            "AND p.id_list_app_usluga=lau.id_list_app_usluga "
            "AND p.date_month="+QString::number(month)+" "
            "AND p.date_year="+QString::number(year)+" ";
        model->setQuery(QSqlQuery(str2));
    }

    if(model->rowCount()==0){  //ООо всё равно пусто, то тогда добавим строчки
        QString str2;
        QSqlQuery query2;
        QStringList column, values;

        column << "id_list_app_usluga" << "date_month" << "date_year"
               << "pokazanie_home"     << "pokazanie_end";

        str2 = "SELECT id_list_app_usluga FROM list_app_usluga WHERE id_apartament=%1"; //все Услуги квартиры
        str2 = str2.arg(id_apartament);

        if (query2.exec(str2)){
            while (query2.next()){ // переберём все строчки
                values.clear();
                values << query2.value(0).toString() << QString::number(month) << QString::number(year)
                       << QString::number(0)         << QString::number(0);
                add("pokazanie",column,values);
            }
        } else{
            qDebug()<<query2.lastError();
            LogOut.logout(query2.lastError().text());
        }
        model = ModelPokazanie(id_apartament, month, year); // И теперь попробуем ещё раз получить не пустую модель

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

    QString str;
    str = "SELECT p.id_pokazanie, u.name, p.pokazanie_home, p.pokazanie_end, t.tarif "
            " FROM list_app_usluga lau, usluga u, tarif t, pokazanie p "
            " WHERE "
            " lau.id_usluga = u.id_usluga "
            " AND t.id_usluga = u.id_usluga "
            " AND u.type_usluga = 1 "
            " AND NOT u.id_usluga = 3"
            " AND lau.id_apartament="+QString::number(id_apartament)+" "
            " AND p.id_list_app_usluga=lau.id_list_app_usluga "
            " AND t.month_t=p.date_month "
            " AND p.date_month="+QString::number(month)+" "
            " AND p.date_year=t.year_t "
            " AND p.date_year="+QString::number(year)+" ";

    model->setMyQuery(str);
    return model;
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
    int out=-1;

    QString str;
    QSqlQuery query;

    str = "SELECT type_usluga FROM usluga WHERE id_usluga=%1";
    str = str.arg(id_usluga);

    if (query.exec(str)){
      if (query.next()){
          out =  query.value(0).toInt();
      }else{
          qDebug()<<"not record" << str;
          out = -1;
      }
    } else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        out = -1;
    }


    return out;
}

int BD::is_idListAppUsluga(int id_apartament, int id_usluga)
{
    int out=-1;
    QString str;

    str = "SELECT id_list_app_usluga FROM list_app_usluga WHERE id_usluga=%1 AND id_apartament=%2";
    str = str.arg(id_usluga)
            .arg(id_apartament);

    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toInt();
    }
    return out;
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
//    qDebug()<<"new void";
    QString str;
    QSqlQuery query;
    str =   " SELECT id_pokazanie, pokazanie_end "
            " FROM pokazanie p, list_app_usluga lau "
            " WHERE lau.id_apartament = %1 "
                " AND lau.id_list_app_usluga = p.id_list_app_usluga "
                " AND p.date_year = %3 "
                " AND p.date_month = %2";
    str = str.arg(id_apartament)
                .arg(next_month(month))
                .arg(next_year(month,year));

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
//    qDebug()<<"new int";
    int id_new = -1;
    int month_old = -1, year_old = -1, month, year, id_ListApart=-1;
    QString str;
    QStringList column, value;
    QSqlQuery query;


    //  найдём дату текущих показаний
    str = "SELECT date_month, date_year,id_list_app_usluga FROM pokazanie WHERE id_pokazanie=%1";
    str = str.arg(id_pok_old);

    if (query.exec(str)){
        if (query.next()){
            month_old =  query.value(0).toInt();
            year_old = query.value(1).toInt();
            id_ListApart = query.value(2).toInt();
        }else{
            qDebug()<<"not record" << str;
        }
    } else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
    }
    //проверим на коректность
    month = next_month(month_old);
    year = next_year(month_old,year_old);
    if(month == -1 ||year  == -1){
        return -1;
    }

    //Проверим на существование показаний на след месяц
    str = "SELECT id_pokazanie FROM pokazanie "
            "WHERE date_month = %1 AND date_year=%2 AND id_list_app_usluga=%3";
    str = str.arg(QString::number(month))
            .arg(QString::number(year))
            .arg(QString::number(id_ListApart));
    QVariant t = SelectFromTable(str);
    if(t.toInt() == -1){//если нет записей
        //добавим новое показание на след месяц
//        qDebug("new");
        column.clear();
        column<<"id_list_app_usluga"<<"date_month"<<"date_year"<<"pokazanie_home"<<"pokazanie_end";
        value.append(QString::number(id_ListApart));
        value.append(QString::number(month));
        value.append(QString::number(year));
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

int BD::next_month(int m)
{
    int out = -1;
    if(m > 0 && m < 12){
        out = m + 1;
    }else if(m==12){
        out = 1;
    }
    return out;
}

int BD::next_year(int m, int y)
{
    int out = -1;

    if(y<0){
        return -1;
    }

    if(next_month(m) == 1){
        out = y+1;
    }else if(next_month(m) != -1){
        out = y;
    }

    return out;
}

int BD::previous_month(int m)
{
    int out = -1;
    if(m > 0 && m <= 12){
        out = m - 1;
    }else if(m==1){
        out = 12;
    }
    return out;
}

int BD::previous_year(int y, int m)
{
    int out = -1;

    if(y<0){
        return -1;
    }

    if(previous_month(m) == 12){
        out = y-1;
    }else if(previous_month(m) != -1){
        out = y;
    }

    return out;
}

void BD::DataProcessing(int id_org, int id_home, int month, int year)
{
//    QString str;
    Q_UNUSED(id_org);
    Q_UNUSED(id_home);
    Q_UNUSED(month);
    Q_UNUSED(year);


//    str = "SELECT id_credited, "
}

void BD::CreditedOfService(int month, int year, int id_apartament)  //расчёт для квартиры
{
    QList<int> ListService;
    QString str;
    QSqlQuery query;
    ListService = is_ApartamentService(id_apartament);                  // список ид услуг по квартире
    for(int i=0;i<ListService.size();i++){
        int id_list_ap_usl = 0;
        id_list_ap_usl = is_idListAppUsluga(id_apartament, ListService[i]);

        str = "SELECT id_credited FROM credited WHERE id_list_app_usluga=%1 AND month=%2 AND year=%3";
        str = str.arg(id_list_ap_usl)
                .arg(month)
                .arg(year);

        // перебирается все услуги всех квартир
        // начисление по услугам
        if (query.exec(str)){
            double cred = CreditedOfApartament(month,year,id_list_ap_usl);
            if (query.next()){
                QVariant credR = SelectFromTable("SELECT credited_of_service FROM credited "
                                "WHERE id_credited = "+query.value(0).toString());
                if(cred != credR.toDouble()){
                    UpdateTable("credited","credited_of_service",
                                QString::number(cred),
                                "id_credited", query.value(0).toString()
                    );
                }
            }else{
                QStringList column,value;
                column<<"id_list_app_usluga"<<"month"<<"year"<<"credited_of_service";
                value<<QString::number(id_list_ap_usl)<<
                    QString::number(month)<< QString::number(year)<<
                    QString::number(CreditedOfApartament(month,year,id_list_ap_usl));
                add("credited",column,value);
            }
        } else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
        }

    }
    CreditedForApartament(id_apartament,month,year);  //расчёт общей оплаты и задолжности
}

int BD::is_Pokazanie(int id_list_app_usluga, int month, int year)
{
    QString str;
    int out = -1;

    str = "SELECT pokazanie_home FROM pokazanie WHERE id_list_app_usluga=%1 AND date_month=%2 AND date_year=%3";
    str = str.arg(id_list_app_usluga)
            .arg(next_month(month))
            .arg(next_year(month,year));
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

double BD::CreditedOfApartament(int month, int year, int id_list_app_usluga) // начисление услуга-квартрира-дата
{
    QString str;
    QSqlQuery query;
    int type_usluga, id_usluga;
    double tarif = 0, out = -1;

    //получим тип услуги ид услуги и тариф
    str = "SELECT u.type_usluga, u.id_usluga FROM usluga u, list_app_usluga lau "
            "WHERE u.id_usluga=lau.id_usluga AND lau.id_list_app_usluga=%1";
    str = str.arg(id_list_app_usluga);

    if (query.exec(str)){
      if (query.next()){
          type_usluga = query.value(0).toInt();
          id_usluga = query.value(1).toInt();
          table_tariff tbl_tariff;
          tarif = tbl_tariff.is_Tariff(month,year,id_usluga);// Получаем тариф
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
        out = PaymentCounters(id_list_app_usluga,month,year);
    }
    if(type_usluga == 2){ //на кв метр
        QString str2;
        QSqlQuery query2;
        str2 = "SELECT total_area FROM apartament a, list_app_usluga lau "
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

double BD::CreditedForReport(int month, int year, int id_apartament, int id_usluga)
{
    QString str;
    double out = -1;

    str = "SELECT credited_of_service FROM credited c, list_app_usluga lau "
            "WHERE month=%1 AND year=%2 AND lau.id_list_app_usluga=c.id_list_app_usluga "
            "AND lau.id_apartament=%3 AND lau.id_usluga=%4 ";
    str = str.arg(month)
            .arg(year)
            .arg(id_apartament)
            .arg(id_usluga);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toDouble();
    }
    return out;
}

void BD::CreditedForApartament(int id_apart, int month, int year)
{
    QString str;
    QSqlQuery query;
    double cred_with_count=0.0, cred_out_count=0.0;

    // К оплате по услугам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.month=%1 AND c.year=%2 "
            "AND c.id_list_app_usluga=lau.id_list_app_usluga "
            "AND lau.id_usluga=u.id_usluga "
            "AND NOT u.type_usluga='1' "
            "AND lau.id_apartament=%3";
    str = str.arg(month)
            .arg(year)
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        cred_out_count = t.toDouble();
    }

    // К оплате по счётчикам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.month=%1 AND c.year=%2 "
                "AND c.id_list_app_usluga=lau.id_list_app_usluga "
                "AND lau.id_usluga=u.id_usluga "
                "AND u.type_usluga=1 "
                "AND lau.id_apartament=%3";
    str = str.arg(month)
            .arg(year)
            .arg(id_apart);
    t = SelectFromTable(str);
    if(!t.isNull()){
        cred_with_count = t.toDouble();
    }
    str = "SELECT id_credited_of_apartament FROM credited_of_apartament "
            "WHERE month=%1 AND year=%2 AND id_apartament=%3";
    str = str.arg(month)
            .arg(year)
            .arg(id_apart);
    if (query.exec(str)){
        if (query.next()){
            UpdateTable("credited_of_apartament","credited_with_counter",QString::number(cred_with_count),
                        "id_credited_of_apartament",query.value(0).toString());
            UpdateTable("credited_of_apartament","credited_out_counter",QString::number(cred_out_count),
                        "id_credited_of_apartament",query.value(0).toString());
        }else{
            QStringList column, value;
            column<<"id_apartament"<<"month"<<"year"<<"credited_with_counter"<<"credited_out_counter";
            value<<QString::number(id_apart)<<QString::number(month)<<QString::number(year)
                <<QString::number(cred_with_count)<<QString::number(cred_out_count);
            add("credited_of_apartament",column,value);
        }
    }else{
        qDebug()<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
    }

    PaymentOfDebt(id_apart,month,year);// Расчёт долга
}

double BD::AmountToPay(int id_apart, int month, int year)
{
    QString str;
    double out = 0.0, debt = 0.0;

    str="SELECT debt FROM debt WHERE  year_debt=%1 AND month_debt=%2 AND id_apartament=%3";
    str = str.arg(year)
            .arg(month)
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        debt = t.toDouble();
    }

    out = debt + AmountForServices(id_apart,month,year);
    return out;
}

void BD::PaymentOfDebt(int id_apart, int month, int year)
{
    QString str;
    QSqlQuery query;
    double debt = 0.0, bay = 0.0;



    // --------- К оплате в прошлом месяце
    debt += AmountToPay(id_apart, previous_month(month),previous_year(year,month));

    //---------- Долг за счётчики в этом месяце
    str="SELECT credited_with_counter FROM credited_of_apartament WHERE  year=%1 AND month=%2 AND id_apartament=%3";
    str = str.arg(year)
            .arg(month)
            .arg(id_apart);
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        debt += t.toDouble();
    }
    //---------- Оплата после 25 числа прошлого месяца по 25 число этого месяца
    str =   "SELECT bay, DATE(bay_date,'%Y') as y_bay, DATE(bay_date,'%M') as m_bay, DATE(bay_date,'%d') as d_bay  "
            "FROM bay "
            "WHERE "
            "   (( "
            "     (y_bay = %3 AND m_bay = %4 AND d_bay >=25 ) "
            "     OR (y_bay = %1 AND m_bay = %2 AND d_bay =1 ) "
            "   ) "
            "   OR "
            "   ( "
            "     (y_bay = %1 AND m_bay = %2 AND d_bay >=1) "
            "     OR (y_bay = %1 AND m_bay = %2 AND d_bay < 25 ) "
            "   )) "
            "AND id_apartament=%5";
    str = str.arg(year)
            .arg(month)
            .arg(previous_year(year,month))
            .arg(previous_month(month))
            .arg(id_apart);
    if (query.exec(str)){
        while (query.next()){
            qDebug() << query.value(0).toDouble();
            bay = bay + query.value(0).toDouble();
        }
    }else{
        qDebug()<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
    }
    debt = debt - bay;

    //-----обновление/добавление долга
    str = "SELECT id_debt, debt FROM debt WHERE year_debt=%1 AND month_debt=%2 AND id_apartament=%3";
    str = str.arg(year)
            .arg(month)
            .arg(id_apart);
    if (query.exec(str)){
        if (query.next()){
            if(QString::number(debt,'f',2) == query.value(1).toString()){
                return;
            }else{
                UpdateTable("debt","debt",QString::number(debt,'f',2),"id_debt",query.value(0).toString());
            }
        }else{
            qDebug()<<str<<" line not found ";
            QStringList column,value;
            column << "year_debt" << "month_debt" << "id_apartament" << "debt";
            value << QString::number(year) << QString::number(month)
                  << QString::number(id_apart) << QString::number(debt,'f',2);
            add("debt",column,value);
        }
    }else{
        qDebug()<<query.lastError()<<str;
        LogOut.logout(query.lastError().text());
        return;
    }
}
//-------------------------------------------------------------------------------------------
QString BD::is_Debt(int id_apart, int month, int year)
{
    QString str;
    double debt = 0.0;
    QString out= "" ;

    str="SELECT debt FROM debt WHERE  year_debt=%1 AND month_debt=%2 AND id_apartament=%3";
    str = str.arg(year)
            .arg(month)
            .arg(id_apart);
//    qDebug()<<str;
    QVariant t = SelectFromTable(str);
    if (!t.isNull()){
        debt = t.toDouble();
    }

    if (debt>0){
        out = QObject::trUtf8("Ваш долг составляет:  ") + QString::number(debt,'f',2) + QObject::trUtf8(" p. ");
    }else if(debt<0){
        out = QObject::trUtf8("Ваша переплата составляет:  ") + QString::number(debt,'f',2) + QObject::trUtf8(" p. ");
    }
//    qDebug()<<out;
    return out;
}


double BD::AmountForServices(int id_apart, int month, int year)
{
    QString str;
    double out = 0;

    str="SELECT credited_out_counter FROM credited_of_apartament WHERE  year=%1 AND month=%2 AND id_apartament=%3";
    str = str.arg(year)
            .arg(month)
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

double BD::PaymentCounters(int id_list_app_usluga, int month, int year)  //расчёт оплаты за счётчик
{
    QString str;
    QSqlQuery query;
    int id_usluga, id_apartament;
    double tarif,tarif2,norma, out = -1;

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
    str = "SELECT t.tarif, t.tarif2, t.norm FROM tarif t, list_app_usluga lau "
            "WHERE lau.id_usluga=t.id_usluga AND lau.id_list_app_usluga=%1 AND t.year_t=%2 AND t.month_t=%3";
    str = str.arg(id_list_app_usluga)
            .arg(previous_year(year,month))
            .arg(previous_month(month));
    if (query.exec(str)){
      if (query.next()){
            tarif = query.value(0).toDouble();
            tarif2 = query.value(1).toDouble();
            norma = query.value(2).toDouble();
      }else{
          return -1;
      }
    } else{
        qDebug()<<"CreditedOfApartament"<<query.lastError();
        LogOut.logout(query.lastError().text());
        return -1;
    }

    if(is_TypeUsluga(id_usluga)==1){
        str = "SELECT pokazanie_end, pokazanie_home FROM pokazanie "
                "WHERE id_list_app_usluga=%1 "
                "AND date_month=%2 AND date_year=%3";
        str = str.arg(id_list_app_usluga)
                .arg(month)
                .arg(year);
        if (query.exec(str)){
            if (query.next()){
                int count = query.value(0).toDouble() - query.value(1).toDouble();
                if(isElectroUsluga(id_usluga)){// Электричество
                    if(count <= norma*is_RealMen(id_apartament,year,month)|| norma==0 || is_pensioner_living_alone(id_apartament)){
                        out = tarif * count;
                    }else if (count > norma*is_RealMen(id_apartament,year,month)&& norma!=0){
                        out = norma * is_RealMen(id_apartament,year,month) * (tarif - tarif2) + count * tarif2;
                    }
                }else{
                    out = tarif * count;
                }

            }else{
                return -1;
            }
        }else{
            qDebug()<<query.lastError();
            LogOut.logout(query.lastError().text());
            return -1;
        }
    }

//    qDebug()<<"COUNT = "<<out;
    return out;
}



QString BD::delete_Bay(int id_apartament, int year, int month, int day)
{
    QString str;
    QSqlQuery query;
    QString out = "";

    str =   "DELETE FROM bay "
            "WHERE id_apartament=%1 AND DATE(bay_date,'%Y')=%2 ANDDATE(bay_date,'%M')=%3 AND DATE(bay_date,'%d')=%4";
    str = str.arg(id_apartament)
            .arg(year)
            .arg(month)
            .arg(day);
    if (!query.exec(str)){
        out = query.lastError().text();
    }
    return out;
}

void BD::SumCount(int id_pokazanie, bool New/* = false*/) //Расчёт показаний канализации
{
    QString str;
    QSqlQuery query;
    int month, year, id_app, value = 0, id_sumpok;

    str = " SELECT date_month, date_year, id_apartament"            //Дата и квартира
          " FROM pokazanie p, list_app_usluga lau "
          " WHERE id_pokazanie = %1 "
            " AND lau.id_list_app_usluga = p.id_list_app_usluga ";
    str = str.arg(id_pokazanie);

    if (query.exec(str)){
        if (query.next()){
            month = query.value(0).toInt();
            year = query.value(1).toInt();
            id_app = query.value(2).toInt();
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
            " WHERE date_month = %1 "
            " AND date_year = %2 "
            " AND id_apartament = %3 "
            " AND lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND (id_usluga=1 OR id_usluga=2)";                // Воды и ГВС

    if (!New){
        str = str.arg(month)
                .arg(year)
                .arg(id_app);
    }else{
        str = str.arg(next_month(month))
                .arg(next_year(month,year))
                .arg(id_app);
    }

    if (query.exec(str)){
        while (query.next()){
            value = value + query.value(0).toInt();             //Всё суммируем
        }
    }else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    }
    qDebug()<<"VALUE = "<<value;

    if (!New){
        str = " SELECT id_pokazanie, pokazanie_end ";
    } else {
        str = " SELECT id_pokazanie, pokazanie_home ";
    }
    str +=  " FROM pokazanie p, list_app_usluga lau "
            " WHERE lau.id_list_app_usluga = p.id_list_app_usluga "
            " AND date_month = %1 "
            " AND date_year = %2 "
            " AND id_apartament = %3"
            " AND id_usluga = 3";                           //ид счётчика канализации
    if (!New){
        str = str.arg(month)
                .arg(year)
                .arg(id_app);
    } else {
        str = str.arg(next_month(month))
                .arg(next_year(month,year))
                .arg(id_app);
    }

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
            new_pokazanie(id_sumpok,QString::number(value)); //показание на начало след месяца
        }else{           
            return;
        }
    }else{
        qDebug()<<query.lastError();
        LogOut.logout(query.lastError().text());
        return;
    } 
}

void BD::UpdateOrganization(QString name, QString bank, QString acc, int idorg)
{
    UpdateTable("organiz","name",name,"id_organiz",QString::number(idorg));
    UpdateTable("organiz","bank",bank,"id_organiz",QString::number(idorg));
    UpdateTable("organiz","sett_account",acc,"id_organiz",QString::number(idorg));
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
    QString str;
    QSqlQuery query;
    int m = 0,y = 0 ,id_lau = 0;

    str = "SELECT id_list_app_usluga, date_year, date_month FROM pokazanie WHERE id_pokazanie = %1";
    str = str.arg(id_pokazanie);
    if (query.exec(str)) {
        if (query.next()){
            id_lau = query.value(0).toInt();
            y = query.value(1).toInt();
            m = query.value(2).toInt();
        }else{
            qDebug()<< "not found in" << str;
        }
    }else{
        qDebug() << query.lastError().text();
        LogOut.logout(query.lastError().text());
    }

//    qDebug() << "old pok id" << id_pokazanie;
    UpdateTable("pokazanie","pokazanie_home",QString::number(new_pokazanie),"id_pokazanie",QString::number(is_IdPokazanie(id_lau,m,y)));
    SumCount(id_pokazanie,true);
//    qDebug()<<trUtf8("Обновление");
}

int BD::is_IdPokazanie(int id_list_app_usluga, int month, int year)
{
    QString str;
    int out = -1;

    str = "SELECT id_pokazanie FROM pokazanie WHERE id_list_app_usluga = %1 AND date_month = %2 AND date_year = %3";
    str = str.arg(id_list_app_usluga)
            .arg(next_month(month))
            .arg(next_year(month,year));
    QVariant t = SelectFromTable(str);
    if(!t.isNull()){
        out = t.toInt();
    }
//    qDebug() << "new pok id" << out;
    return out;
}

void BD::UpdateHome(int id_home, QString home)
{
    UpdateTable("homes","name",home,"id_homes",QString::number(id_home));
}
