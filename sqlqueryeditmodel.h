#ifndef SQLQUERYEDITMODEL_H
#define SQLQUERYEDITMODEL_H

#include <QSqlQueryModel>
//#include "bd.h"

class SqlQueryEditModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    SqlQueryEditModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setMyQuery(QString str_query);

private:
    bool setPokazanie(int Id, const QString &value);
    void refresh();
    QString myQuery;
signals:
    void sgn_EditPokazanie(int id_pok, QString value);
public slots:

};

#endif
