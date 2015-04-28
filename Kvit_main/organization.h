#ifndef ORGANIZATION_H
#define ORGANIZATION_H

#include <QObject>
#include "bd.h"

class Organization : public QObject
{
    Q_OBJECT

public:
    explicit Organization();
    explicit Organization(int id_apartament);

    bool New(QString name, QString bank, QString sett_account, QString inn);
    QSqlQueryModel* ModelAllOrganization();
    static QSqlQueryModel* ModelAllOrganizationOnlyName();
    QString getName();
    QString getBank();
    QString getINN();
    int getId();
    void setId(int id_organization);
    void setName(QString NameOrganization);
    void deleteFromDB();
    void Update(QString name = "", QString bank="", QString acc="", QString inn="");
private:
    int m_id;
    QString m_name;
    QString m_bank;
    QString m_sett_account;
    QString m_inn;
    BD db;
};

#endif // ORGANIZATION_H
