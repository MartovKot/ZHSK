#include "editapartmentmodel.h"
#include <QDebug>

EditApartmentModel::EditApartmentModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}


Qt::ItemFlags EditApartmentModel::flags(
        const QModelIndex &index) const
{
    if (!index.isValid() || index.column()==0){
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

}

bool EditApartmentModel::setData(const QModelIndex &index, const QVariant &value, int  role )
{
    if (index.isValid() && role == Qt::EditRole) {
        emit sgn_EditApartament(index.column(),value.toString());
        return true;
     }
     return false;
}
