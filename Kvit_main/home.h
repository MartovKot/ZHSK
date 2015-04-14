#ifndef HOME_H
#define HOME_H

#include <QObject>
#include <QSqlQueryModel>
#include "organization.h"
#include "apartment.h"
#include "bd.h"

class Home : public QObject
{
    Q_OBJECT
public:
    explicit Home(const int id, QObject *parent = 0);
    explicit Home(const QString &name, QObject *parent = 0);
    ~Home();

    QString getName();
    int getId();

    void deleteFromDB();
    void rename(QString new_name);
    static QSqlQueryModel* ModelAllHome();
    const Organization* organization();
    const QList<Apartment *> apartments();

private:
    void setApartment();
    QString m_name;
    int m_id;
    Organization *m_organization;
    QList<Apartment *> m_apartment;
    void set_default();
    QList<int> isListIdApartament();
signals:

public slots:

};

#endif // HOME_H
