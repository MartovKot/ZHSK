#ifndef SQLQUERYEDITMODEL_H
#define SQLQUERYEDITMODEL_H

#include <QSqlQueryModel>

class SqlQueryEditModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    SqlQueryEditModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setMyQuery(QString str_query);
    void setEditColumn(QList<int> lst);

private:
    bool setPokazanie(int Id, const QString &value);
    void refresh();
    QString myQuery;
    QList<int> lst_editcolumn;
signals:
    void sgn_EditPokazanie(int id_pok, QString value);
    void sgn_RefreshModel(QAbstractTableModel* model);
    void sgn_EditApartament(int column, QString value);
public slots:

};

#endif
