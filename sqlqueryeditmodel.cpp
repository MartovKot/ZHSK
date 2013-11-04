#include <QtSql>

#include "sqlqueryeditmodel.h"

SqlQueryEditModel::SqlQueryEditModel(QObject *parent)
    : QSqlQueryModel(parent)
{

}

Qt::ItemFlags SqlQueryEditModel::flags(
        const QModelIndex &index) const
{   
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 2){
        flags |= Qt::ItemIsEditable;

    }
    return flags;
}

bool SqlQueryEditModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{

    if(index.column()!=2){
        return false;
    }

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    int id = data(primaryKeyIndex).toInt();

    clear();

    bool ok = false;
    if (index.column() == 2){
        ok = setPokazanie(id,value.toString());
    }
    refresh();

    return ok;
}


void SqlQueryEditModel::refresh()
{
    setMyQuery(myQuery);
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
    removeColumn(2);
    removeColumn(3);
    setHeaderData(0, Qt::Horizontal, QObject::trUtf8("№"));
    setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Счётчик"));
    setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Показазия текущие"));
}


