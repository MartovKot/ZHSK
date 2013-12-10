#include "table_bay.h"

table_bay::table_bay()
{
    db = new BD();
}

table_bay::~table_bay()
{
    delete db;
}

bool table_bay::add_line(QString id_apartament, QString y_bay, QString m_bay, QString d_bay, QString bay)
{
    QStringList column, value;

    qint64 timeInUnix;
    const qint64 MS_COEF = 1000;
    QDate date;
    date.setDate(y_bay.toInt(), m_bay.toInt(), d_bay.toInt());
    QDateTime datetime;
    datetime.setTimeSpec(Qt::OffsetFromUTC);
    datetime.setDate(date);
    timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;

    column<<"id_apartament"<<"bay_date"<<"bay";
    value<<id_apartament<<QString::number(timeInUnix)<<bay;
    if (db->add("bay",column,value)!=0){
        return false;
    }
    return true;
}

QSqlQueryModel* table_bay::ModelBay(int id_apartament)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString str;
    str = "SELECT  strftime('%d',bay_date,'unixepoch'), strftime('%m',bay_date,'unixepoch'),strftime('%Y',bay_date,'unixepoch'), bay FROM bay WHERE id_apartament = "+QString::number(id_apartament);
    model->setQuery(QSqlQuery(str));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("День"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Месяц"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Год"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Сумма"));
    return model;
}
