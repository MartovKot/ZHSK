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
    QString version;

    version = getDatabaseVersion();

    if (version == "1.5"){
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

    major = version.left(2);
    minor = version.mid(3,2);
    subversion = version.right(3);

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
    QueryExecute("VACUUM");

}

QSqlError BD::SelectFromTable(QString str, QString *var)
{
    *var = "";
    QSqlQuery query;
    if (query.exec(str)){
        if (query.next()){
            *var = query.value(0).toString();
        }
    } else{
        qDebug() <<"ERROR "<< "d04157f41fd3c5c4dee3f0e0dd41baed \n" << query.lastError() << "\n" << str;
    }
    return query.lastError();
}

QSqlError BD::QueryExecute(QString str)
{
    QSqlQuery query;
    if (!query.exec(str)){
        qDebug() << "1098339a5e20736b5ba8f1eb1839f4ea" << query.lastError() << str;
    }
    return query.lastError();
}

QSqlError BD::add(QString table, QString column, QString value, int mode/*=0*/)
{
    QStringList C_sl, V_sl;
    C_sl<<column;
    V_sl<<value;
    return add(table,C_sl,V_sl,mode);
}

QSqlError BD::add(QString table,QStringList column, QStringList value, int mode/*=0*/)
{
    QString strF;
    QSqlQuery query;
    int i;

    switch(mode){
    case 0:
        strF = "INSERT";
        break;
    case 1:
        strF = "REPLACE";
        break;
    case 2:
        strF = "INSERT OR REPLACE";
        break;
    case 3:
        strF = "INSERT OR ROLLBACK";
        break;
    case 4:
        strF = "INSERT OR ABORT";
        break;
    case 5:
        strF = "INSERT OR FAIL";
        break;
    case 6:
        strF = "INSERT OR IGNORE";
        break;
    default:
        strF = "INSERT";
        break;
    }

    strF = strF + " INTO "+table+" ( ";
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
    }
    return query.lastError();
}

void BD::UpdateTable(QString table, QString column, QString value, QString where1, QString where2  )
{
    QString str;
    QSqlQuery query;
    QString value_temp = "";

    SelectFromTable("SELECT "+column+" FROM "+table+" WHERE "+where1+" = '"+where2+"'",&value_temp);
    if (value_temp != value ){
        str = "UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'";
        str = str.arg(table)
                    .arg(column)
                    .arg(value)
                    .arg(where1)
                    .arg(where2);
        if (!query.exec(str))
        {
            qDebug()<<"bf206b5a653a24ca32249bfd24515d4e" <<"Eror  "<<query.lastError()<<"\n"<<str;
        }
    }
}

void BD::UpdateTable(QString table, QString column, QString value, QString where_column1, QString where_value1, QString where_column2, QString where_value2)
{
    QString str;
    QSqlQuery query;
    QString value_temp = "";
    SelectFromTable("SELECT "+column+" FROM "+table+" WHERE "+where_column1+" = '"+where_value1+"'" +
                        " AND "+where_column2+" = '"+where_value2+"'",&value_temp);
    if (value_temp != value ){
        str = "UPDATE %1 SET %2 = '%3' WHERE %4 = '%5' AND %6 = '%7'";
        str = str.arg(table)
                    .arg(column)
                    .arg(value)
                    .arg(where_column1)
                    .arg(where_value1)
                    .arg(where_column2)
                    .arg(where_value2);
        if (!query.exec(str))
        {
            qDebug()<<"bf206b5a653a24ca32249bfd24515d4e" <<"Eror  "<<query.lastError()<<"\n"<<str;
        }
    }
}

QSqlError BD::DeleteLine(QString table, QString id_name, int id_line)
{
    QString str;
    QSqlQuery query;

    str = "DELETE FROM %1 WHERE %2 = %3";
    str = str.arg(table)
            .arg(id_name)
            .arg(id_line);

    if (!query.exec(str)){
        qDebug() << "eb1c9c25246c60d6db683a79c4e941ad " << str << query.lastError();
    }

    return query.lastError();
}

QString BD::getDatabaseVersion()
{
    QString str;
    QSqlQuery query;
    QString res = "";

    str = "SELECT version FROM version";
    if (query.exec(str)) {
        if (query.next()){
            res = query.value(0).toString();
        }else{
            res = "";
        }
    }else{
        res = "";
    }
    return res;
}
