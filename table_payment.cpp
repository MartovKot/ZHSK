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

    qint64 timeInUnix;
    const qint64 MS_COEF = 1000;
    QDate date;
    date.setDate(y_payment.toInt(), m_payment.toInt(), d_payment.toInt());
//    qDebug() << date;
    QDateTime datetime;
    datetime.setTimeSpec(Qt::OffsetFromUTC);
    datetime.setDate(date);
//    qDebug() << datetime  << datetime.timeSpec();
    timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;
//    qDebug() << timeInUnix;

    column<<"id_apartament"<<"payment_date"<<"payment";
    value<<id_apartament<<QString::number(timeInUnix)<<payment;
    if (db->add("payment",column,value)!=0){
        return false;
    }
    return true;
}

QSqlQueryModel* Table_Payment::ModelPayment(int id_apartament)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT  strftime('%d',payment_date,'unixepoch'), strftime('%m',payment_date,'unixepoch'),strftime('%Y',payment_date,'unixepoch'), payment FROM payment WHERE id_apartament = "+QString::number(id_apartament);
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("День"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Месяц"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Год"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Сумма"));

    return model;
}

QString Table_Payment::delete_Payment(int id_apartament, int year, int month, int day)
{
    QString str;
    QSqlQuery query;
    QString out = "";

    str =   "DELETE FROM payment "
            "WHERE id_apartament=%1 AND DATE(payment_date,'%Y')=%2 ANDDATE(payment_date,'%M')=%3 AND DATE(payment_date,'%d')=%4";
    str = str.arg(id_apartament)
            .arg(year)
            .arg(month)
            .arg(day);
    if (!query.exec(str)){
        out = query.lastError().text();
    }
    return out;
}