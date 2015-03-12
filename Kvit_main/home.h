#ifndef HOME_H
#define HOME_H

#include <QObject>
#include "bd.h"

class Home : public QObject
{
    Q_OBJECT
public:
    explicit Home();
    void setId(int id);
    void setName(QString name);

    QString getName();
    int getId();

    void deleteFromDB();
    QSqlQueryModel* ModelAllHome();
    void rename(QString new_name);
private:
    QString m_name;
    int m_id;
    BD db;
    void set_default();
signals:

public slots:

};

#endif // HOME_H
