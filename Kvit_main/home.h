#ifndef HOME_H
#define HOME_H

#include <QObject>
#include <QSqlQueryModel>
#include "organization.h"
#include "bd.h"

class Home : public QObject
{
    Q_OBJECT
public:
    explicit Home(int id);
    explicit Home(const QString &name);
    ~Home();

    QString getName();
    int getId();

    void deleteFromDB();
    void rename(QString new_name);
    static QSqlQueryModel* ModelAllHome();
//    Organization* organization();
private:
    QString m_name;
    int m_id;
//    Organization *m_organization;
    void set_default();
signals:

public slots:

};

#endif // HOME_H
