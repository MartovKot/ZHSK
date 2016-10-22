#ifndef MODELAPARTAMENT_H
#define MODELAPARTAMENT_H

#include <QSqlQueryModel>
#include <QColor>
#include <QSqlQuery>
#include "dateofunixformat.h"
#include "bd.h"
#include "apartment.h"

class ModelApartament : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelApartament(QObject *parent = 0);
    void setIdApartment(int id);
    QVariant data(const QModelIndex &index,
                   int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index,
                    const QVariant &value, int role);
private:
    int m_IdApartment;
    void refresh();
    void UpdateApartament(QString column,QString value, int idapart);           //Обновление таблицы с квартирами
    void UpdateMenInApartment(QString column,QString value, int idapart);
    void AddLineMenInApartment(int id_apartment);
};

#endif // MODELAPARTAMENT_H
