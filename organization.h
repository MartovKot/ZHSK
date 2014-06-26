#ifndef ORGANIZATION_H
#define ORGANIZATION_H

#include <QObject>
#include "bd.h"

class Organization : public QObject
{
    Q_OBJECT

public:
    Organization();
    Organization(int id_apartament);
    bool New(QString name, QString bank, QString sett_account, QString inn);
    QSqlQueryModel* ModelAllOrganization();
    QString getName();
    QString getBank();
private:
    int id;
    QString m_name;
    QString m_bank;
    QString m_sett_account;
    QString m_inn;
    BD db;
};

#endif // ORGANIZATION_H
