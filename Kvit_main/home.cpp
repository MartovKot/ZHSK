#include "home.h"

Home::Home(int id, QObject *parent):
    QObject(parent)
{
    BD::SelectFromTable( "SELECT name FROM homes WHERE id_homes = " + QString::number(id),&m_name );

    if (m_name != ""){
        m_id = id;
    }else{
        set_default();
    }

    QString id_org;
    BD::SelectFromTable( "SELECT id_organiz FROM apartament WHERE id_homes = " + QString::number(m_id) + " LIMIT 1",
                         &id_org);
    m_organization = new Organization(id_org.toInt(),this);
}

Home::Home(const QString &name, QObject *parent):
    QObject(parent)
{
    QString id;
    BD::SelectFromTable( "SELECT id_homes FROM homes WHERE name = '" + name + "'",&id);
    if(id != ""){
        m_id = id.toInt();
        m_name = name;
    }else{
        set_default();
    }
}

Home::~Home()
{

}

void Home::set_default()
{
    m_name = "";
    m_id = -1;
}

QString Home::getName()
{
    return m_name;
}

int Home::getId()
{
    return m_id;
}

void Home::deleteFromDB()
{
    BD::DeleteLine("homes","id_homes",m_id);
}

QSqlQueryModel* Home::ModelAllHome()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_homes, name  FROM homes"));

    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Адрес"));

    return model;
}

const Organization *Home::organization()
{
    return m_organization;
}

void Home::rename(QString new_name)
{
    BD::UpdateTable("homes","name",new_name,"id_homes",QString::number(m_id));
}
