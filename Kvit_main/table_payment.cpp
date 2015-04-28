#include "table_payment.h"

Table_Payment::Table_Payment()
{
    db = new BD();
}

Table_Payment::~Table_Payment()
{
    delete db;
}

bool Table_Payment::add_line(QString id_apartament, QString y_payment, QString m_payment, QString d_payment, QString payment)
{
    QStringList column, value;
    DateOfUnixFormat date(y_payment.toInt(), m_payment.toInt(), d_payment.toInt());

    column << "id_apartament" << "payment_date" << "payment";
    value << id_apartament << QString::number(date.Second()) << payment;
    if (db->add("payment",column,value)!=0){
        return false;
    }
    return true;
}

QSqlQueryModel* Table_Payment::ModelPayment(int id_apartament)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT  strftime('%d %m %Y',payment_date,'unixepoch'), "
//            " strftime('%m',payment_date,'unixepoch'), "
//            " strftime('%Y',payment_date,'unixepoch'), "
            " payment "
            " FROM payment WHERE id_apartament = "+QString::number(id_apartament);
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Дата"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Сумма"));
//    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("День"));
//    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Месяц"));
//    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Год"));
//    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Сумма"));

    return model;
}

QString Table_Payment::delete_Payment(int id_apartament, int year, int month, int day)
{
    QString str;
    QSqlQuery query;
    QString out = "";
    DateOfUnixFormat date(year,month,day);

    str = "DELETE FROM payment "
            "WHERE id_apartament=%1 AND payment_date=%2";

    str = str.arg(id_apartament)
            .arg(date.Second());
    if (!query.exec(str)){
        out = query.lastError().text();
    }
    return out;
}


//QVariant Table_Payment::SumLastMonthPayment()
//{
//    //---------- Оплата после 25 числа прошлого месяца по 25 число этого месяца
////    str =   "SELECT payment, DATE(payment_date,'%Y') as y_payment, DATE(payment_date,'%M') as m_payment, DATE(payment_date,'%d') as d_payment  "
////            "FROM payment "
////            "WHERE "
////            "   (( "
////            "     (y_payment = %3 AND m_payment = %4 AND d_payment >=25 ) "
////            "     OR (y_payment = %1 AND m_payment = %2 AND d_payment =1 ) "
////            "   ) "
////            "   OR "
////            "   ( "
////            "     (y_payment = %1 AND m_payment = %2 AND d_payment >=1) "
////            "     OR (y_payment = %1 AND m_payment = %2 AND d_payment < 25 ) "
////            "   )) "
////            "AND id_apartament=%5";
////    str = str.arg(date.year())
////            .arg(date.month())
////            .arg(previous_year(date.year(),date.month()))
////            .arg(previous_month(date.month()))
////            .arg(id_apart);
////    if (query.exec(str)){
////        while (query.next()){
////            payment = payment + query.value(0).toDouble();
////        }
////    }else{
////        LogOut.logout(query.lastError().text());
////    }
//}
