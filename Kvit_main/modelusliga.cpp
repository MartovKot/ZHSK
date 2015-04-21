#include "modelusliga.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

ModelUsliga::ModelUsliga(QObject *parent)
                 : QSqlQueryModel(parent) {
    m_IdApartment = -1;
//    refresh();
}

Qt::ItemFlags ModelUsliga::flags(
    const QModelIndex &index) const {

     Qt::ItemFlags flags = QSqlQueryModel::flags(index);
//     if (index.column() >= 1 && index.column() < 4)
//         flags |= Qt::ItemIsEditable;
     if (index.column() == 1)
         flags |= Qt::ItemIsUserCheckable;
     return flags;
}

QVariant ModelUsliga::data(
             const QModelIndex &index,
             int role) const {

     QVariant value = QSqlQueryModel::data(index, role);


//     if (index.column() == 1){
//         qDebug() << value.toInt() << role;
//     }
     switch (role) {

     case Qt::DisplayRole: // Данные для отображения
     case Qt::EditRole:    // Данные для редактирования
         if (index.column() == 1){
             return value.toInt() != 0 ? tr("Подключено") : tr("Не подключено");
         }else {
             return value;
         }

//     case Qt::TextColorRole: // Цвет текста
//         if(index.column() == 1)
//             return qVariantFromValue(QColor(Qt::blue));
//         else
//             return value;

//     case Qt::TextAlignmentRole: // Выравнивание
//         if(index.column() == 3)
//             return int(Qt::AlignRight | Qt::AlignVCenter);
//         else if(index.column() == 2 || index.column() == 4)
//             return int(Qt::AlignHCenter | Qt::AlignVCenter);
//         else
//             return int(Qt::AlignLeft | Qt::AlignVCenter);

//     case Qt::FontRole: // Шрифт
//         if(index.column() == 1) {
//             QFont font = QFont("Helvetica", 10, QFont::Bold);
//             return qVariantFromValue(font);
//         }else
//             return value;

     case Qt::BackgroundColorRole: {  // Цвет фона
//         int a = (index.row() % 2) ? 14 : 0;
         if(index.column() == 0)
             return qVariantFromValue(QColor(200,220,255));
         else if(index.column() == 1 && QSqlQueryModel::data(index).toInt() != 0)
             return qVariantFromValue(QColor(230, 250, 130));
//         else if(index.column() == 1 && index.row()%2 == 0)
//             return qVariantFromValue(QColor(200,220-a,255-a));
         else
             return value;
     }
     case Qt::CheckStateRole:  // Галочка
         if (index.column() == 1)
             return (QSqlQueryModel::data(index).toInt() != 0) ?
                     Qt::Checked : Qt::Unchecked;
         else
             return value;

//     case Qt::SizeHintRole:  // Размер ячейки
//         if (index.column() == 0)
//             return QSize(70, 10);
//         if (index.column() == 4)
//             return QSize(60, 10);
//         else
//             return QSize(110, 10);
     }
     return value;
}

 bool ModelUsliga::setData(
             const QModelIndex &index,
              const QVariant &value,
             int /* role */) {
    if (index.column() != 1)
         return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
             index.row(), 2);
    int id_usluga = QSqlQueryModel::data(primaryKeyIndex).toInt();

    //clear(); // Если надо полностью перерисовать таблицу.

    bool ok;
    QSqlQuery query;
    if (index.column() == 1) {
        if (value.toInt() == 0){
            Apartment apartment(m_IdApartment);
            BD::DeleteLine("list_app_usluga","id_list_app_usluga",apartment.isIdListApartamentServise(id_usluga));
        }else{
            QStringList value;
            QStringList column;
            column << "id_apartament" << "id_usluga";
            value << QString::number(m_IdApartment) << QString::number(id_usluga);
            BD::add("list_app_usluga",column,value);
        }
     }
     ok = query.exec();
     refresh();
     return ok;
}

void ModelUsliga::refresh()
{
    setQuery(" SELECT  name, "
             " CASE WHEN lau.id_list_app_usluga THEN 1 ELSE 0 END new_field, "
             " u.id_usluga"
             " FROM usluga u  "
             " LEFT OUTER JOIN  list_app_usluga lau ON u.id_usluga = lau.id_usluga "
             " AND  lau.id_apartament = " + QString::number(m_IdApartment)
             );

    setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Услуга"));
    setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Подключение"));
}

void ModelUsliga::setIdApartment(int id)
{
    m_IdApartment = id;
    refresh();
}
