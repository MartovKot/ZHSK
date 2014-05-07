#include "table_tariff.h"

table_tariff::table_tariff()
{
    db = new BD();
}

table_tariff::~table_tariff()
{
    delete db;
}

float table_tariff::is_Tariff(int usluga, QDate date, int tariff/*=1*/)
{
    QString str, strF;
    double out = -1;
    DateOfUnixFormat t_date(date.year(),date.month(),1);

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
    strF = str.arg(t_date.Second())
            .arg(usluga);
    QVariant t = db->SelectFromTable(strF);
    if (!t.isNull()){
        out = t.toDouble();
    }
    out = QString::number(out,'f',2).toDouble();
    QString t3 = QString::number(out,'f',2);
    return out;

}

QSqlQueryModel* table_tariff::ModelTarifTabl(int year, int month)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    QVariant count_tarif;
    DateOfUnixFormat date(year,month,1);

    //----Вынести в другое место----
    str = "SELECT COUNT() FROM tariff "
            " WHERE tariff_date = " + QString::number(date.Second());
    count_tarif = db->SelectFromTable(str);
    if(!count_tarif.isNull()){
        if(count_tarif.toInt()==0){
            QString str2 = "INSERT INTO tariff(id_usluga, tariff_date) SELECT id_usluga, %1 FROM usluga";
            str2 = str2.arg(QString::number(date.Second()));
            QSqlQuery query2;
            if(query2.exec(str2)){

            }else{
                qDebug() << "aebb8b3a6e4299f530efd00de3e80361" <<query2.lastError();
            }
        }
    }
    //-----конец

    str = "SELECT t.id_tariff, u.name, t.tariff, t.tariff2, t.norm FROM tariff t, usluga u "
            " WHERE t.tariff_date = " + QString::number(date.Second()) + " AND u.id_usluga=t.id_usluga";
    model->setQuery(QSqlQuery(str));
    if (model->lastError().number() != -1){
        qDebug() << "d79f962a16168e6156e982b602b533dc" << model->lastError();
    }
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
    DateOfUnixFormat date(year,month,1);

    str ="SELECT tariff, tariff2, norm, id_usluga FROM tariff WHERE tariff_date=%1";
    str = str.arg(QString::number(date.Second(-1)));
    if (query.exec(str)){
        while(query.next()){
            QString str2;
            QSqlQuery query2;
            str2 = "SELECT id_tariff FROM tariff WHERE tariff_date=%1 AND id_usluga=%2";
            str2 = str2.arg(date.Second())
                    .arg(query.value(3).toInt());
            if (query2.exec(str2)){
              if (query2.next()){
                  table_tariff tbl_tariff;
                  tbl_tariff.UpdateTarif(query.value(0).toString(),query.value(1).toString()
                                          ,query.value(2).toString(),query2.value(0).toInt());
              }else{
                  QStringList column, value;
                  column << "tariff" <<"tariff2" <<"norm"<<"id_usluga"<<"tariff_date";
                  value << query.value(0).toString() << query.value(1).toString() << query.value(2).toString()
                           << query.value(3).toString() << QString::number(date.Second());
                  db->add("tariff",column,value);
              }
            } else{
                qDebug()<<"02e81a7a2124e38f486e726ea5422e69"<<query2.lastError();
                return -1;
            }
        }
    }else{
        qDebug()<<"a9fca9fafc74e63119a07736a8236acb"<<query.lastError()<<str;
        return -1;
    }

    return 0;
}
