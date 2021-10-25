#include "organization.h"

Organization::Organization(const QString &name,QObject *parent):
    QObject(parent)
{
    QString id;
    BD::SelectFromTable("SELECT id_organiz FROM organiz WHERE name = '" + name + "'",&id);
    if (id.toInt() == -1){
        setDefault();
        return;
    }
    m_id = id.toInt();

    m_name = name;

    BD::SelectFromTable("SELECT bank FROM organiz WHERE name = '" + name + "'",&m_bank);
    BD::SelectFromTable("SELECT sett_account FROM organiz WHERE name = '" + name + "'",&m_sett_account);
    BD::SelectFromTable("SELECT inn FROM organiz WHERE name = '" + name + "'",&m_inn);


}

Organization::Organization(int id,QObject *parent):
    QObject(parent)
{
    BD::SelectFromTable("SELECT name FROM organiz WHERE id_organiz = "+QString::number(id),&m_name);
    if(m_name == ""){
        setDefault();
        return;
    }
    BD::SelectFromTable("SELECT bank FROM organiz WHERE id_organiz = "+QString::number(id),&m_bank);
    BD::SelectFromTable("SELECT sett_account FROM organiz WHERE id_organiz = "+QString::number(id),&m_sett_account);
    BD::SelectFromTable("SELECT inn FROM organiz WHERE id_organiz = "+QString::number(id),&m_inn);
    BD::SelectFromTable("SELECT bik FROM organiz WHERE id_organiz = "+QString::number(id),&m_bik);

    m_id = id;
}

Organization::~Organization()
{

}

bool Organization::New(QString name, QString bank, QString sett_account, QString inn)
{
    QStringList column, value;

    column << "name" << "bank" << "sett_account" << "inn";
    value << name << bank << sett_account << inn;
    if (BD::add("organiz",column,value).number() != 0){
        return false;
    }
    return true;
}

QSqlQueryModel *Organization::ModelAllOrganizationFull()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_organiz, name, bank,sett_account, inn, bik  FROM organiz"));

    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Организация"));
    model->setHeaderData(2,Qt::Horizontal,QObject::trUtf8("Банк"));
    model->setHeaderData(3,Qt::Horizontal,QObject::trUtf8("Лицевой счёт"));
    model->setHeaderData(4,Qt::Horizontal,QObject::trUtf8("ИНН"));
    model->setHeaderData(5,Qt::Horizontal,QObject::trUtf8("БИК"));


    return model;
}

QSqlQueryModel *Organization::ModelAllOrganizationName()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT name FROM organiz"));

    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Организация"));

    return model;
}

int Organization::getId() const
{
    return m_id;
}

void Organization::deleteFromDB()
{
    BD::DeleteLine("organiz","id_organiz", m_id);
}

void Organization::Update(QString name, QString bank, QString acc, QString inn, QString bik)
{
    BD::UpdateTable("organiz", "name", name, "id_organiz", QString::number(m_id));
    BD::UpdateTable("organiz", "bank", bank, "id_organiz", QString::number(m_id));
    BD::UpdateTable("organiz", "sett_account", acc, "id_organiz", QString::number(m_id));
    BD::UpdateTable("organiz", "inn", inn, "id_organiz", QString::number(m_id));
    BD::UpdateTable("organiz", "bik", bik, "id_organiz", QString::number(m_id));
}
QString Organization::inn() const
{
    return m_inn;
}

QString Organization::bank() const
{
    return m_bank + " " + m_sett_account;
}

QString Organization::name() const
{
    return m_name;
}

QString Organization::bik() const
{
    return m_bik;
}

void Organization::setDefault()
{
    m_id = -1;
    m_name = "";
    m_bank = "";
    m_sett_account = "";
    m_inn = "";
    m_bik = "";
}
