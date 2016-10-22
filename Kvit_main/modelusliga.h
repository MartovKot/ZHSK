#ifndef MODELUSLIGA_H
#define MODELUSLIGA_H

#include <QSqlQueryModel>
#include <QTableView>
#include <QDate>
#include "bd.h"
#include "apartment.h"

class ModelUsliga : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelUsliga (QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                   int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,
                    const QVariant &value, int role);
    void setIdApartment(int id);
private:
     void refresh();
     int m_IdApartment;
     BD db;
};

#endif // MODELUSLIGA_H
