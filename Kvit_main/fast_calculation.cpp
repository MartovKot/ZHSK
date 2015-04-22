#include "fast_calculation.h"

Fast_Calculation::Fast_Calculation(const int &id_home, DateOfUnixFormat date)
{
    home = new Home(id_home,this);
    m_date = date;
}


Fast_Calculation::~Fast_Calculation()
{
    delete home;
}

DateOfUnixFormat Fast_Calculation::date() const
{
    return m_date;
}

void Fast_Calculation::setDate(const DateOfUnixFormat &date)
{
    m_date = date;
}

void Fast_Calculation::fullCalc()
{
    QString str;
    QList<QStringList> result_tabl;

    str = "SELECT a.id_apartament, u.id_usluga, u.type_usluga, t.tariff, t.tariff2, t.norm "
          " FROM apartament a, usluga u, list_app_usluga lau, tariff t "
          " WHERE a.id_apartament = lau.id_apartament "
          " AND u.id_usluga = lau.id_usluga "
          " AND t.id_usluga = u.id_usluga "
          " AND t.tariff_date = '%1' ";
    str = str.arg(m_date.Second_first_day());

    QSqlQuery query;
    if (query.exec(str)){
        while (query.next()){
            QStringList list;
            list << query.value(0).toString() << query.value(1).toString() << query.value(2).toString()
                 << query.value(3).toString() << query.value(4).toString() << query.value(5).toString() << "";
            result_tabl.append(list);
        }
    }

    for(int i=0;i<result_tabl.size();i++){
        QString t = calcOfService(result_tabl.at(i));
        result_tabl[i][6] = t;
    }
    recordInDB_CredOfApart(result_tabl);
    calcOfDebt(); // Расчёт долга
}

QString Fast_Calculation::calcOfService(const QStringList &row)
{
    QString out = "";
    Apartment apartment(row.at(0).toInt());
    double t;

    switch(row.at(2).toInt()){
    case 1: //счётчики
        out = calcOfCounters(row);
        break;
    case 2: //на кв метр
        t = row.at(3).toDouble() * apartment.getTotalArea();
        out = QString::number(t);
        break;
    case 3: //на человека
        t = row.at(3).toDouble() * apartment.getRealMen(m_date);
        out = QString::number(t);
        break;
    case 4: //на квартиру
        out = row.at(3);
        break;
    }
    return out;
}

QString Fast_Calculation::calcOfCounters(const QStringList &row)
{
    QString out;
    Apartment apartment(row.at(0).toInt());

    QString norma;
    int id_usluga;
    QString tariff;
    QString tariff2;


    id_usluga = row.at(1).toInt();

    tariff = row.at(3);
    tariff2 = row.at(4);
    norma = row.at(5);
    //пенсионеры
    if (apartment.getIslivingAlonePensioner()){
        Service service(id_usluga);
        if (service.isElectro()){
            norma = 75;
        }else if(service.isElectroDay()){
            norma = 55;
        }else if(service.isElectroNight()){
            norma = 20;
        }
    }

    QString str;
    QSqlQuery query;
    str =   " SELECT p.pokazanie_end, p.pokazanie_home FROM pokazanie p, apartament a, list_app_usluga lau "
            " WHERE date_pokazanie = '%1' "
            " AND a.id_apartament = '%2' "
            " AND p.id_list_app_usluga = lau.id_list_app_usluga "
            " AND a.id_apartament = lau.id_apartament "
            " AND lau.id_usluga = '%3' ";
    str = str.arg(m_date.Second_first_day())
            .arg(apartment.getId())
            .arg(id_usluga);
    if (query.exec(str)){
        if (query.next()){
            int count = query.value(0).toInt() - query.value(1).toInt();
            if (count <= norma.toDouble() * apartment.getRealMen(m_date) || norma.toDouble() == 0){
                double t;
                t = tariff.toDouble() * count;
                out = QString::number(t);
            }else{
                double t;
                t = norma.toDouble() * apartment.getRealMen(m_date) *
                    (tariff.toDouble() - tariff2.toDouble()) + count * tariff2.toDouble();
                out = QString::number(t);
            }

        }
    }else{
        qDebug()<<"91748951796616cc5b01273b7a07c8f7"<<query.lastError();
    }

    return out;
}

void Fast_Calculation::recordInDB_CredOfApart(const QList<QStringList> &table)
{

    QHash<int, QStringList> hash_tabl;

    for (int i=0;i<table.size();i++){

        QStringList val;
        if(table.at(i).at(1).toInt() == 1){
            val << table.at(i).at(6) << "";
        }else{
            val << "" << table.at(i).at(6);
        }

        if(hash_tabl.contains(table.at(i).at(0).toInt())){
            QStringList lst = hash_tabl.value(table.at(i).at(0).toInt());
            val[0] = QString::number(val.at(0).toDouble() + lst.at(0).toDouble());
            val[1] = QString::number(val.at(1).toDouble() + lst.at(1).toDouble());
            hash_tabl[table.at(i).at(0).toInt()] = val;
        }else{
            hash_tabl.insert(table.at(i).at(0).toInt(),val);
        }
    }

    if(hash_tabl.size() <= 0){
        return;
    }

    QString str;
    str = "INSERT OR REPLACE INTO 'credited_of_apartament' ";
    QList<int> list;
    list = hash_tabl.keys();

    str = str +
            " SELECT "
            " NULL AS 'id_credited_of_apartament', "
            " '" + QString::number(list.at(0)) + "' AS 'id_apartament', "
            " '" + QString::number(m_date.Second_first_day()) + "' AS 'date_credited_of_apartament', "
            " '" + hash_tabl.value(list.at(0)).at(0) + "' AS 'credited_with_counter', "
            " '" + hash_tabl.value(list.at(0)).at(1) + "' AS 'credited_out_counter' ";

    for (int i=1;i<list.size();i++){
        str = str +
            " UNION SELECT "
            " NULL, "
            " '" + QString::number(list.at(i)) + "', "
            " '" + QString::number(m_date.Second_first_day()) + "', "
            " '" + hash_tabl.value(list.at(i)).at(0) + "', "
            " '" + hash_tabl.value(list.at(i)).at(1) + "' ";
    }
    BD db;
    db.QueryExecute(str);

    str = "";
    for (int i=0;i<table.size();i++){
        if (table.at(i).at(2).toInt() == 1){
            continue;
        }
        Apartment apartament(table.at(i).at(0).toInt());
        if (str == "") {
            str =
            " INSERT OR REPLACE INTO 'credited' (id_list_app_usluga,date_credited,credited_of_service) "
            " SELECT %1, %2, %3";
            str = str.arg(apartament.isIdListApartamentServise(table.at(i).at(1).toInt()))
                    .arg(m_date.Second_first_day())
                    .arg(table.at(i).at(6));
            continue;
        }

        str = str + " UNION SELECT %1, %2, %3 ";
        str = str.arg(apartament.isIdListApartamentServise(table.at(i).at(1).toInt()))
                .arg(m_date.Second_first_day())
                .arg(table.at(i).at(6));
    }
    db.QueryExecute(str);
}

void Fast_Calculation::calcOfDebt()
{
    QString str;

    str =
    " INSERT OR REPLACE INTO 'debt' (debt) "
    " SELECT "
            " d.debt + credited_with_counter + CASE "
                " WHEN  (EXISTS( SELECT payment FROM payment WHERE payment_date >= '%1' AND payment_date <= '%2'))"
                    " THEN (SELECT SUM(payment) FROM payment WHERE payment_date >= '%1' AND payment_date <= '%2')"
                    " ELSE '0'"
                " END as pay"
    " FROM debt d, credited_of_apartament coa, payment p "
    " WHERE  d.date_debt = '%1'"
        " AND date_credited_of_apartament = '%2'"
        " AND d.id_apartament = coa.id_apartament"
        " AND d.id_apartament = p.id_apartament"
    " GROUP BY pay";

    str = str.arg(m_date.Second_first_day(-1))
            .arg(m_date.Second_first_day());

    qDebug() << str;

    BD::QueryExecute(str);
}

double Fast_Calculation::AmountForServices(int id_apart, qint64 u_date)
{
    QString str;
    QString out;

    str = "SELECT credited_out_counter FROM credited_of_apartament "
          "WHERE date_credited_of_apartament='%1' AND id_apartament=%2";
    str = str.arg(u_date)
            .arg(id_apart);
    BD db;
    db.SelectFromTable(str, &out);

    return out.toDouble();
}

QString Fast_Calculation::Debt(int id_apart, DateOfUnixFormat date)
{
    QString str;
    QString debt;
    QString out= "" ;

    str="SELECT debt FROM debt WHERE  date_debt=%1 AND id_apartament=%3";
    str = str.arg(date.Second())
            .arg(id_apart);
    BD::SelectFromTable(str,&debt);

    if (debt.toDouble()>0.0){
        out = QObject::trUtf8("Ваш долг составляет:  ")
                + QString::number(debt.toDouble(),'f',2) + QObject::trUtf8(" p. ");
    }else if(debt<0){
        out = QObject::trUtf8("Ваша переплата составляет:  ")
                + QString::number(debt.toDouble(),'f',2) + QObject::trUtf8(" p. ");
    }
    return out;
}

double Fast_Calculation::mDebt(int idApart, qint64 uDate)
{
    QString str;
    QString debt;

    str="SELECT debt FROM debt WHERE  date_debt=%1 AND id_apartament=%3";
    str = str.arg(uDate)
            .arg(idApart);
    BD::SelectFromTable(str,&debt);

    return debt.toDouble();
}

QString Fast_Calculation::CreditedForReport(int id_apartament, int id_usluga, DateOfUnixFormat date)
{
    QString str;
    QString out;

    str = "SELECT credited_of_service FROM credited c, list_app_usluga lau "
            "WHERE date_credited=%1 AND lau.id_list_app_usluga=c.id_list_app_usluga "
            "AND lau.id_apartament=%2 AND lau.id_usluga=%3 ";
    str = str.arg(date.Second())
            .arg(id_apartament)
            .arg(id_usluga);
    BD::SelectFromTable(str,&out);

    return out;

}

double Fast_Calculation::AmountToPay(int id_apart, qint64 u_date)
{ 
    double out;
    out = mDebt(id_apart,u_date);
    out += AmountForServices(id_apart,u_date);
    return out;
}
