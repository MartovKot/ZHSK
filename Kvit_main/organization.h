#ifndef ORGANIZATION_H
#define ORGANIZATION_H

#include <QObject>
#include "bd.h"

class Organization : public QObject
{
    Q_OBJECT

public:
    explicit Organization(const QString &name, QObject *parent = 0);
    explicit Organization(int id_organization, QObject *parent = 0);
    ~Organization();

    static bool New(QString name, QString bank, QString sett_account, QString inn);
    static QSqlQueryModel *ModelAllOrganizationFull();
    static QSqlQueryModel *ModelAllOrganizationName();

    int getId() const;
    void deleteFromDB();
    void Update(QString name = "", QString bank="", QString acc="", QString inn="");
    QString inn() const;
    QString bank() const;
    QString name() const;

private:
    int m_id;
    QString m_name;
    QString m_bank;
    QString m_sett_account;
    QString m_inn;
    void setDefault();
};

#endif // ORGANIZATION_H
