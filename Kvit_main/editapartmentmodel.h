#ifndef EDITAPARTMENTMODEL_H
#define EDITAPARTMENTMODEL_H

#include <QSqlQueryModel>


class EditApartmentModel : public QSqlQueryModel
{
    Q_OBJECT
public:

    EditApartmentModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
signals:
    void sgn_EditApartament(int column, QString value);
};

#endif // EDITAPARTMENTMODEL_H
