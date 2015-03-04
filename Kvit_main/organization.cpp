#include "organization.h"

Organization::Organization()
{
    setDefault();
}

Organization::Organization(int id_apartament)
{
    QString id;
    db.SelectFromTable("SELECT id_organiz FROM apartament WHERE id_apartament = "+QString::number(id_apartament),&id);
    if (id == ""){
        setId(id.toInt());
    }else{
        setDefault();
    }
}

bool Organization::New(QString name, QString bank, QString sett_account, QString inn)
{
    QStringList column, value;

    column << "name" << "bank" << "sett_account" << "inn";
    value << name << bank << sett_account << inn;
    if (db.add("organiz",column,value) != 0){
        return false;
    }
    return true;
}

QSqlQueryModel* Organization::ModelAllOrganization()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_organiz, name, bank,sett_account, inn  FROM organiz"));

    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Организация"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Банк"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Лицевой счёт"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("ИНН"));

    return model;
}

QString Organization::getBank()
{
    return m_bank + " " + m_sett_account;
}

QString Organization::getName()
{
    return m_name;
}

QString Organization::getINN()
{
    return m_inn;
}
int Organization::getId()
{
    return m_id;
}

void Organization::setId(int id)
{
    db.SelectFromTable("SELECT name FROM organiz WHERE id_organiz = "+QString::number(id),&m_name);
    db.SelectFromTable("SELECT bank FROM organiz WHERE id_organiz = "+QString::number(id),&m_bank);
    db.SelectFromTable("SELECT sett_account FROM organiz WHERE id_organiz = "+QString::number(id),&m_sett_account);
    db.SelectFromTable("SELECT inn FROM organiz WHERE id_organiz = "+QString::number(id),&m_inn);
    m_id = id;
}

void Organization::setName(QString NameOrganization)
{

    m_name = NameOrganization;
    db.SelectFromTable("SELECT bank FROM organiz WHERE name = '" + NameOrganization + "'",&m_bank);
    db.SelectFromTable("SELECT sett_account FROM organiz WHERE name = '" + NameOrganization + "'",&m_sett_account);
    db.SelectFromTable("SELECT inn FROM organiz WHERE name = '" + NameOrganization + "'",&m_inn);

    QString id;
    db.SelectFromTable("SELECT id_organiz FROM organiz WHERE name = '" + NameOrganization + "'",&id);
    m_id = id.toInt();

}

void Organization::deleteFromDB()
{
    db.DeleteLine("organiz","id_organiz", m_id);
}

void Organization::Update(QString name, QString bank, QString acc, QString inn)
{
    db.UpdateTable("organiz", "name", name, "id_organiz", QString::number(m_id));
    db.UpdateTable("organiz", "bank", bank, "id_organiz", QString::number(m_id));
    db.UpdateTable("organiz", "sett_account", acc, "id_organiz", QString::number(m_id));
    db.UpdateTable("organiz", "inn", inn, "id_organiz", QString::number(m_id));
}

void Organization::setDefault()
{
    m_id = -1;
    m_name = "";
    m_bank = "";
    m_sett_account = "";
    m_inn = "";
}
