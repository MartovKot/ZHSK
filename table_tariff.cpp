#include "table_tariff.h"
#include "my_function.h"

table_tariff::table_tariff()
{
    db = new BD();
}

table_tariff::~table_tariff()
{
    delete db;
}

double table_tariff::is_Tariff(int month, int year, int usluga, int tariff/*=1*/)
{
    QString str, strF;
    double out = -1;

    switch (tariff) {
    case 1:
        str = "SELECT tariff ";
        break;
    case 2:
        str = "SELECT tariff2 ";
        break;
    default:
        break;
    }
    str += "FROM tariff WHERE id_usluga=%2  AND tariff_date=%1 ";
    strF = str.arg(IsDateOfUnix(year,month,1))
            .arg(usluga);
    QVariant t = db->SelectFromTable(strF);
    if (!t.isNull()){
        out = t.toDouble();
    }
    return out;
}

QSqlQueryModel* table_tariff::ModelTarifTabl(int year, int month)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    QVariant count_tarif;

    //----Вынести в другое место----
    str = "SELECT COUNT() FROM tariff "
            " WHERE tariff_date = "+QString::number(IsDateOfUnix(year,month,1));
    count_tarif = db->SelectFromTable(str);
    if(!count_tarif.isNull()){
        if(count_tarif.toInt()==0){
            QString str2 = "INSERT INTO tariff(id_usluga, tariff_date) SELECT id_usluga, '%1','%2' FROM usluga";
            str2 = str2.arg(QString::number(year))
                    .arg(QString::number(month));
            QSqlQuery query2;
            if(query2.exec(str2)){

            }else{
                qDebug()<<query2.lastError();
            }
        }
    }
    //-----конец

    str = "SELECT t.id_tarif, u.name, t.tarif, t.tarif2, t.norm FROM tarif t, usluga u "
            " WHERE t.year_t = " + QString::number(year) +
            " AND t.month_t=" + QString::number(month) + " AND u.id_usluga=t.id_usluga";
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Услуга"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Тариф"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Тариф2"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("Норма"));

    return model;
}

//-----------------------------------------------------------------------------------------------------
void table_tariff::UpdateTarif(QString tarif, QString tarif2, QString norma, int idtarif)
{
    db->UpdateTable("tariff","tariff",tarif,"id_tariff",QString::number(idtarif));
    db->UpdateTable("tariff","tariff2",tarif2,"id_tariff",QString::number(idtarif));
    db->UpdateTable("tariff","norm",norma,"id_tariff",QString::number(idtarif));
}

int table_tariff::FillTarif(int month, int year)
{
    QString str;
    QSqlQuery query;

    str ="SELECT tarif, tarif2, norm, id_usluga FROM tarif WHERE month_t=%1 AND year_t=%2";
    str = str.arg(db->previous_month(month))
            .arg(db->previous_year(year,month));
    if (query.exec(str)){

        while(query.next()){
            QString str2;
            QSqlQuery query2;
            str2 = "SELECT id_tarif FROM tarif WHERE month_t=%1 AND year_t=%2 AND id_usluga=%3";

            str2 = str2.arg(month)
                    .arg(year)
                    .arg(query.value(3).toInt());
            if (query2.exec(str2)){
              if (query2.next()){
                  table_tariff tbl_tariff;
                  tbl_tariff.UpdateTarif(query.value(0).toString(),query.value(1).toString()
                                          ,query.value(2).toString(),query2.value(0).toInt());
              }else{
                  QStringList column, value;
                  column << "tarif" <<"tarif2" <<"norm"<<"id_usluga"<<"year_t"<<"month_t";
                  value << query.value(0).toString() << query.value(1).toString() << query.value(2).toString()
                           << query.value(3).toString() <<QString::number(year) << QString::number(month);
                  db->add("tarif",column,value);
              }
            } else{
                qDebug()<<"CreditedOfApartament"<<query2.lastError();
                return -1;
            }
        }
    }else{
        qDebug()<<query.lastError()<<str;
        return -1;
    }

    return 0;
}
