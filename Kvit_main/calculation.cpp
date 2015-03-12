#include "calculation.h"

Calculation::Calculation(QObject *parent) : QObject(parent)
{

}

Calculation::~Calculation()
{

}

QVariant Calculation::CreditedOfApartament(int id_list_app_usluga, DateOfUnixFormat date) // начисление услуга-квартрира-дата
{
    QString str;
    QSqlQuery query;
    int type_usluga;
    int id_apartment;
    double tarif = 0;
    QVariant out;

    //получим тип услуги ид услуги и тариф
    str = "SELECT u.type_usluga, u.id_usluga, lau.id_apartament FROM usluga u, list_app_usluga lau "
            "WHERE u.id_usluga=lau.id_usluga AND lau.id_list_app_usluga=%1";
    str = str.arg(id_list_app_usluga);

    if (query.exec(str)){
        if (query.next()){
            int id_usluga;
            type_usluga = query.value(0).toInt();
            id_usluga = query.value(1).toInt();
            id_apartment = query.value(2).toInt();

            table_tariff tbl_tariff;
            tarif = tbl_tariff.is_Tariff(id_usluga,date);// Получаем тариф
        }else{
            qDebug() << "not record" << "af43d615b46af5a458e710e8e8f5b61e" << str;
            return -1;
        }
    } else{
        qDebug() << "CreditedOfApartament" << query.lastError();
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
                qDebug() << "not record" << str;
                return -1;
            }
        } else{
            qDebug() << "CreditedOfApartament" << query.lastError();
            return -1;
        }
    }
    if(type_usluga == 3){ //на человека
        Apartment apartment;
        apartment.setId(id_apartment);
        out = tarif * apartment.getRealMen(date);
    }
    if(type_usluga == 4){ //на квартиру
        out = tarif;
    }
    return out;
}

void Calculation::CreditedOfService(int id_apartament, DateOfUnixFormat date)  //расчёт для квартиры
{
    QList<int> ListService;
    QString str;
    QSqlQuery query;
    Apartment apartment;

    apartment.setId(id_apartament);
    ListService = apartment.getListIdServiceFull();                  // список ид услуг по квартире

    for(int i=0;i<ListService.size();i++){
        int id_list_ap_usl;
        id_list_ap_usl = db.is_idListAppUsluga(id_apartament, ListService[i]);

        str = "SELECT id_credited FROM credited WHERE id_list_app_usluga=%1 AND date_credited=%2 ";
        str = str.arg(id_list_ap_usl)
                .arg(date.Second());

        // начисление по услугам
        if (query.exec(str)){
            QVariant cred = CreditedOfApartament(id_list_ap_usl, date);
            if (query.next()){
                QString credR;
                db.SelectFromTable("SELECT credited_of_service FROM credited "
                                "WHERE id_credited = "+query.value(0).toString(),&credR);
                if(cred.toString() != credR){
                    db.UpdateTable("credited","credited_of_service",
                                cred.toString(),
                                "id_credited", query.value(0).toString()
                    );
                }
            }else{
                QStringList column,value;
                column << "id_list_app_usluga" << "date_credited" << "credited_of_service";
                value << QString::number(id_list_ap_usl) <<
                    QString::number(date.Second()) <<
                       cred.toString();
//                    CreditedOfApartament(id_list_ap_usl,date).toString();
                db.add("credited",column,value);
            }
        } else{
            qDebug()<<query.lastError();
        }

    }
    CreditedForApartament(id_apartament,date);  //расчёт общей оплаты и задолжности
}

double Calculation::PaymentCounters(int id_list_app_usluga, DateOfUnixFormat date)  //расчёт оплаты за счётчик
{
    QString str;
    QSqlQuery query;
    int id_usluga;
    double tarif,tarif2,norma, out = -1;
    DateOfUnixFormat previous_date(date.year(),date.month(),date.day());
    Apartment apartment;
    previous_date = date.addMonths(-1);


    //Получим ид квартиры и ид услуги
    str = "SELECT id_usluga, id_apartament FROM list_app_usluga WHERE id_list_app_usluga=%1";
    str = str.arg(id_list_app_usluga);
    if (query.exec(str)){
        if (query.next()){
            int id_apartament;
            id_usluga = query.value(0).toInt();
            id_apartament = query.value(1).toInt();
            apartment.setId(id_apartament);
        }else{
            qDebug()<<"not record" << str;
            return -1;
        }
    } else{
        qDebug()<<"CreditedOfApartament"<<query.lastError()<<str;
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
        return -1;
    }

    //пенсионеры
    if (apartment.isPensionerLivingAlone()){
        if (db.isElectroUsluga(id_usluga)){
            norma = 75;
        }else if(db.isElectroUslugaDay(id_usluga)){
            norma = 55;
        }else if(db.isElectroUslugaNight(id_usluga)){
            norma = 20;
        }
    }

    if(db.is_TypeUsluga(id_usluga)==1){
        str = "SELECT pokazanie_end, pokazanie_home FROM pokazanie "
                "WHERE id_list_app_usluga=%1 "
                "AND date_pokazanie=%2 ";
        str = str.arg(id_list_app_usluga)
                .arg(date.Second());
        if (query.exec(str)){
            if (query.next()){
                int count = query.value(0).toDouble() - query.value(1).toDouble();
                if (count <= norma * apartment.getRealMen(date) || norma==0){
                    out = tarif * count;
                }else{
                    out = norma * apartment.getRealMen(date) * (tarif - tarif2) + count * tarif2;
                }

            }else{
                return -1;
            }
        }else{
            qDebug()<<"91748951796616cc5b01273b7a07c8f7"<<query.lastError();
            return -1;
        }
    }
    return out;
}

void Calculation::CreditedForApartament(int id_apart, DateOfUnixFormat date)
{
    QString str;
    QSqlQuery query;
    QString cred_with_count;
    QString cred_out_count;


    // К оплате по услугам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.date_credited=%1 "
            "AND c.id_list_app_usluga=lau.id_list_app_usluga "
            "AND lau.id_usluga=u.id_usluga "
            "AND NOT u.type_usluga='1' "
            "AND lau.id_apartament=%2 ";
    str = str.arg(date.Second())
            .arg(id_apart);
    db.SelectFromTable(str,&cred_out_count);


    // К оплате по счётчикам
    str = "SELECT SUM(credited_of_service) FROM credited c, list_app_usluga lau, usluga u "
            "WHERE c.date_credited=%1 "
                "AND c.id_list_app_usluga=lau.id_list_app_usluga "
                "AND lau.id_usluga=u.id_usluga "
                "AND u.type_usluga=1 "
                "AND lau.id_apartament=%2";
    str = str.arg(date.Second())
            .arg(id_apart);
    db.SelectFromTable(str,&cred_with_count);

    str = "SELECT id_credited_of_apartament FROM credited_of_apartament "
            "WHERE date_credited_of_apartament=%1 AND id_apartament=%2 ";
    str = str.arg(date.Second())
            .arg(id_apart);
    if (query.exec(str)){
        if (query.next()){
            db.UpdateTable("credited_of_apartament","credited_with_counter",cred_with_count,
                        "id_credited_of_apartament",query.value(0).toString());
            db.UpdateTable("credited_of_apartament","credited_out_counter",cred_out_count,
                        "id_credited_of_apartament",query.value(0).toString());
        }else{
            QStringList column, value;
            column << "id_apartament"<<"date_credited_of_apartament" << "credited_with_counter" << "credited_out_counter";
            value << QString::number(id_apart) << QString::number(date.Second())
                << cred_with_count << cred_out_count;
            db.add("credited_of_apartament",column,value);
        }
    }else{
        qDebug()<<query.lastError()<<str;
    }
    PaymentOfDebt(id_apart,date.year(),date.month());// Расчёт долга
}

void Calculation::PaymentOfDebt(int id_apart, int year, int month)
{
    QString str;
    QSqlQuery query;
    double debt = 0.0, payment = 0.0;
    DateOfUnixFormat  date(year,month,25);

    // --------- К оплате в прошлом месяце
    debt = debt + AmountToPay(id_apart,date.Second_first_day(-1));
    //qDebug() <<"К оплате в прошлом месяце: " << debt;
    //---------- Долг за счётчики в этом месяце
    str="SELECT credited_with_counter FROM credited_of_apartament WHERE  date_credited_of_apartament=%1 AND id_apartament=%2";
    str = str.arg(date.Second_first_day())
            .arg(id_apart);
    QString temp_debt;
    db.SelectFromTable(str,&temp_debt);

    debt = debt + temp_debt.toDouble();

    //qDebug() << "Долг за счётчики в этом месяце: " << debt << "Сумма: "<< payment ;
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
        qDebug() << query.lastError().text();
    }
    debt = debt - payment;
    //qDebug() << "Оплата после 25 числа прошлого месяца по 25 число этого месяца:" << debt << "Сумма: " << payment;
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
                db.UpdateTable("debt","debt",QString::number(debt,'f',2),"id_debt",query.value(0).toString());
            }
//            qDebug() << "z2";
        }else{
            QStringList column,value;
            column << "date_debt" << "id_apartament" << "debt";
            value << QString::number(date.Second_first_day())
                  << QString::number(id_apart) << QString::number(debt,'f',2);
            db.add("debt",column,value);

//            qDebug() << "z3";
        }
    }else{
        qDebug()<<query.lastError()<<str;
        return;
    }
}

double Calculation::AmountForServices(int id_apart, qint64 u_date)
{
    QString str;
    QString out;

    str = "SELECT credited_out_counter FROM credited_of_apartament "
          "WHERE date_credited_of_apartament='%1' AND id_apartament=%2";
    str = str.arg(u_date)
            .arg(id_apart);
    db.SelectFromTable(str, &out);

    return out.toDouble();
}

double Calculation::AmountToPay(int id_apart, qint64 u_date)
{
    QString str;
    double out = 0.0;
    QString debt;

    str="SELECT debt FROM debt WHERE  date_debt=%1 AND id_apartament=%2";

    str = str.arg(u_date)
            .arg(id_apart);
    db.SelectFromTable(str,&debt);

    out = debt.toDouble() + AmountForServices(id_apart,u_date);
    return out;
}
