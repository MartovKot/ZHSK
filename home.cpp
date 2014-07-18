#include "home.h"

Home::Home()
{
    set_default();
}

void Home::set_default()
{
    m_name = "";
    m_id = -1;
}

void Home::setId(int id)
{
    QVariant t = db.SelectFromTable(
                "SELECT name FROM homes WHERE id_homes = " + QString::number(id)
                );
    if(t.isValid()){
        m_name = t.toString();
        m_id = id;
    }
}

void Home::setName(QString name)
{
    QVariant t = db.SelectFromTable(
                "SELECT id_homes FROM homes WHERE name = '" + name + "'"
                );
    if(t.isValid()){
        m_id = t.toInt();
        m_name = name;
    }
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
    db.DeleteLine("homes","id_homes",m_id);
}
