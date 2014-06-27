#include "home.h"

Home::Home()
{
    m_name = "";
    m_id = -1;
}

void Home::setId(int id)
{
    m_id = id;

    QVariant t = db.SelectFromTable(
                "SELECT name FROM homes WHERE id_homes = "+QString::number(id)
                );
    if(!t.isValid()){
        m_name = t.toString();
    }
}

QString Home::getName()
{
    return m_name;
}
