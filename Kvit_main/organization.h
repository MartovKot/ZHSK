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
//    explicit

    bool New(QString name, QString bank, QString sett_account, QString inn);
    QSqlQueryModel* ModelAllOrganization();
    int getId();
    void setId(int id_organization);
    void deleteFromDB();
    void Update(QString name = "", QString bank="", QString acc="", QString inn="");
    QString inn() const;
    void setInn(const QString &inn);

    QString bank() const;
    void setBank(const QString &bank);

    QString name() const;
    void setName(const QString &name);

private:
    int m_id;
    QString m_name;
    QString m_bank;
    QString m_sett_account;
    QString m_inn;
    BD db;
    void setDefault();
};

#endif // ORGANIZATION_H
