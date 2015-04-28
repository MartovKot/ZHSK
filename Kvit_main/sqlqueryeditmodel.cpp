#include "sqlqueryeditmodel.h"
#include <QDebug>
SqlQueryEditModel::SqlQueryEditModel(QObject *parent)
    : QSqlQueryModel(parent)
{

}

Qt::ItemFlags SqlQueryEditModel::flags(
        const QModelIndex &index) const
{   
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    for (int i=0;i<lst_editcolumn.count();i++){
        if (index.column() == lst_editcolumn.at(i)){
            flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}

bool SqlQueryEditModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    bool ok = false;
    for (int i=0;i<lst_editcolumn.count();i++){
        if(index.column() != lst_editcolumn.at(i)){
            continue;
        }

        QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
        int id = data(primaryKeyIndex).toInt();

        clear();

//        if (index.column() == lst_editcolumn.at(i)){
            ok = setPokazanie(id,value.toString());
            emit sgn_EditApartament(index.column(),value.toString());

//        }
    }
    refresh();
    return ok;
}


void SqlQueryEditModel::refresh()
{
//    qDebug() << this->headerData(2,Qt::Horizontal);
    setQuery(myQuery);
    sgn_RefreshModel(this);

}

bool SqlQueryEditModel::setPokazanie(int Id, const QString &value)
{
    sgn_EditPokazanie(Id, value);
    return true;
}

void SqlQueryEditModel::setMyQuery(QString str_query)
{
    myQuery = str_query;
    setQuery(myQuery);
}

void SqlQueryEditModel::setEditColumn(QList<int> lst)
{
    lst_editcolumn = lst;
}
