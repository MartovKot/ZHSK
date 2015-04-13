#ifndef HOME_H
#define HOME_H

#include <QObject>
#include <QSqlQueryModel>
#include "bd.h"

class Home : public QObject
{
    Q_OBJECT
public:
    explicit Home(int id);
    explicit Home(QString name);

    QString getName();
    int getId();

    void deleteFromDB();
    void rename(QString new_name);
    static QSqlQueryModel* ModelAllHome();
private:
    QString m_name;
    int m_id;
    BD db;
    void set_default();
signals:

public slots:

};

#endif // HOME_H
