#include "modelapartament.h"
#include <QDebug>

ModelApartament::ModelApartament(QObject *parent)
    : QSqlQueryModel(parent)
{
    m_IdApartment = -1;
}

void ModelApartament::setIdApartment(int id)
{
    m_IdApartment = id;
    refresh();
}

Qt::ItemFlags ModelApartament::flags(
    const QModelIndex &index) const {

     Qt::ItemFlags flags = QSqlQueryModel::flags(index);
     if (index.column() == 12){
         flags |= Qt::ItemIsUserCheckable;
     }else{
         flags |= Qt::ItemIsEditable;
     }

     return flags;
}

void ModelApartament::refresh()
{
    DateOfUnixFormat date(QDate::currentDate());
    setQuery("SELECT max(mia.date_men_in_apartament), number, surname, name, patronymic, total_area, \
                    inhabed_area,balkon, loggia, personal_account, mia.real_men, mia.rent_men, mia.reserv_men \
                    ,CASE WHEN hl.id_apartament IS NULL THEN 'FALSE' ELSE 'TRUE' END \
                    FROM apartament a \
                    INNER JOIN men_in_apartament mia ON mia.id_apartament = a.id_apartament \
                    LEFT OUTER JOIN heated_loggia hl ON  hl.id_apartament =  a.id_apartament \
                    WHERE a.id_apartament = " + QString::number(m_IdApartment) + " AND  \
                    mia.date_men_in_apartament <= " + QString::number(date.Second_first_day()));
    removeColumn(0);
    setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Фамилия"));
    setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Имя"));
    setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Отчество"));
    setHeaderData(4,Qt::Horizontal,QObject::trUtf8("Общ. Площадь"));
    setHeaderData(5,Qt::Horizontal,QObject::trUtf8("Жил. Площадь"));
    setHeaderData(6,Qt::Horizontal,QObject::trUtf8("Балкон"));
    setHeaderData(7,Qt::Horizontal,QObject::trUtf8("Лоджия"));
    setHeaderData(8,Qt::Horizontal,QObject::trUtf8("Личн. счёт"));
    setHeaderData(9,Qt::Horizontal,QObject::trUtf8("Проживает"));
    setHeaderData(10,Qt::Horizontal,QObject::trUtf8("Снимает"));
    setHeaderData(11,Qt::Horizontal,QObject::trUtf8("Бронь"));
    setHeaderData(12,Qt::Horizontal,QObject::trUtf8("Тёплая лоджия"));

}

QVariant ModelApartament::data(
             const QModelIndex &index,
             int role) const {

     QVariant value = QSqlQueryModel::data(index, role);
     switch (role) {
         case Qt::DisplayRole: // Данные для отображения
         case Qt::EditRole:    // Данные для редактирования
             if (index.column() == 12){
                 return value.toString() == "TRUE" ? tr("ДА") : tr("НЕТ");
             }else {
                 return value;
             }
//         case Qt::BackgroundColorRole: {  // Цвет фона
//    //         int a = (index.row() % 2) ? 14 : 0;
//             if(index.column() == 0)
//                 return qVariantFromValue(QColor(200,220,255));
//             else if(index.column() == 1 && QSqlQueryModel::data(index).toInt() != 0)
//                 return qVariantFromValue(QColor(230, 250, 130));
//    //         else if(index.column() == 1 && index.row()%2 == 0)
//    //             return qVariantFromValue(QColor(200,220-a,255-a));
//             else
//                 return value;
//         }
         case Qt::CheckStateRole:  // Галочка
             if (index.column() == 12)
                 return (QSqlQueryModel::data(index).toString() == "TRUE") ?
                         Qt::Checked : Qt::Unchecked;
             else
                 return value;
     }
     return value;
}

bool ModelApartament::setData(
            const QModelIndex &index,
             const QVariant &value,
            int role) {
//   clear(); // Если надо полностью перерисовать таблицу.

   bool ok=true;
   if (index.isValid() && index.column() == 12) {
       if (value.toInt() == 0){
           BD::DeleteLine("heated_loggia","id_apartament",m_IdApartment);
       }else{
           BD::add("heated_loggia","id_apartament",QString::number(m_IdApartment));
       }
   }else if (index.isValid() && role == Qt::EditRole){
       switch(index.column()){
       case 0:
           UpdateApartament("number",value.toString(),m_IdApartment);
           break;
       case 1:
           UpdateApartament("surname",value.toString(),m_IdApartment);
           break;
       case 2:
           UpdateApartament("name",value.toString(),m_IdApartment);
           break;
       case 3:
           UpdateApartament("patronymic",value.toString(),m_IdApartment);
           break;
       case 4:
           UpdateApartament("total_area",value.toString(),m_IdApartment);
           break;
       case 5:
           UpdateApartament("inhabed_area",value.toString(),m_IdApartment);
           break;
       case 6:
           UpdateApartament("balkon",value.toString(),m_IdApartment);
           break;
       case 7:
           UpdateApartament("loggia",value.toString(),m_IdApartment);
           break;
       case 8:
           UpdateApartament("personal_account",value.toString(),m_IdApartment);
           break;
       case 9:
           UpdateMenInApartment("real_men",value.toString(),m_IdApartment);
           break;
       case 10:
           UpdateMenInApartment("rent_men",value.toString(),m_IdApartment);
           break;
       case 11:
           UpdateMenInApartment("reserv_men",value.toString(),m_IdApartment);
           break;
       }
   }
    refresh();
    return ok;
}

void ModelApartament::UpdateApartament(QString column, QString value, int idapart)
{
    BD::UpdateTable("apartament",column,value,"id_apartament", QString::number(idapart));
}

void ModelApartament::UpdateMenInApartment(QString column, QString value, int idapart)
{
    QString str;
    DateOfUnixFormat date(QDate::currentDate());


    str = "SELECT COUNT(*) FROM men_in_apartament WHERE id_apartament = %1 AND date_men_in_apartament = %2";
    str = str.arg(idapart)
            .arg(date.Second_first_day());

    QString count = 0;
    BD::SelectFromTable(str,&count);

    if (count.toInt() == 0){//если записей за текущий месяц нету
        AddLineMenInApartment(idapart); //добавим запись
    }
    BD::UpdateTable("men_in_apartament",column,value,
                   "date_men_in_apartament",QString::number(date.Second_first_day()),
                   "id_apartament", QString::number(idapart));

}
void ModelApartament::AddLineMenInApartment(int id_apartment)
{
    QString str;
    DateOfUnixFormat date_now(QDate::currentDate());

    str = "SELECT max(date_men_in_apartament) FROM men_in_apartament WHERE id_apartament = %1";
    str = str.arg(id_apartment);
    QString date;
    BD::SelectFromTable(str,&date);
    QString str2;

    str2 = " INSERT INTO men_in_apartament (id_apartament, real_men, rent_men, reserv_men, date_men_in_apartament ) "
            " SELECT id_apartament, real_men, rent_men, reserv_men, %1 "
            " FROM men_in_apartament"
            " WHERE date_men_in_apartament = %2 AND id_apartament = %3";

    str2 = str2.arg(QString::number(date_now.Second_first_day()))
            .arg(date)
            .arg(id_apartment);
    BD::QueryExecute(str2);
//    }else{
//        qDebug() << "need add code";
    //    }
}
