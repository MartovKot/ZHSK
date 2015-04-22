#include "home.h"

Home::Home(const int id, QObject *parent):
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
    setApartment();
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
    QString id_org;
    BD::SelectFromTable( "SELECT id_organiz FROM apartament WHERE id_homes = " + QString::number(m_id) + " LIMIT 1",
                         &id_org);
    m_organization = new Organization(id_org.toInt(),this);
    setApartment();
}

Home::~Home()
{
    delete m_organization;
    for(int i=0;i<m_apartment.count();i++){
        delete m_apartment.at(i);
    }
}

void Home::set_default()
{
    m_name = "";
    m_id = -1;
}

QList<int> Home::isListIdApartament()
{
    QString str;
    QList<int> out;
    QSqlQuery query;

    str = "SELECT id_apartament FROM apartament WHERE id_homes=%1 AND id_organiz=%2";
    str = str.arg(m_id)
            .arg(m_organization->getId());
    if (query.exec(str)){
      while (query.next()){
          out <<  query.value(0).toInt();
      }
    } else{
        qDebug()<< "709737893f541e722add6cf123fad78e" << query.lastError();
    }

    return out;
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

QSqlQueryModel *Home::ModelAllHomeFull()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT id_homes, name  FROM homes"));

    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("№"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Адрес"));

    return model;
}

QSqlQueryModel *Home::ModelAllHomeName()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT name  FROM homes"));

    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Адрес"));

    return model;
}

bool Home::createNew(QString name)
{
    if (BD::add("homes","name",name).number() != 0){
        return false;
    }
    return true;
}

QSqlQueryModel *Home::ModelAllApartamentNumber()
{
    return Apartment::ModelAllApartment(m_id,m_organization->getId());
}

QSqlQueryModel *Home::ModelAllApartamentNumberWithFIO()
{
    return Apartment::ModelAllApartmentNumFIO(m_id,m_organization->getId());
}

const Organization *Home::organization()
{
    return m_organization;
}

const QList<Apartment *> Home::apartments()
{
    return m_apartment;
}

void Home::setApartment()
{
    QList<int> listIdApartment;
    listIdApartment = isListIdApartament();
    for (int i=0;i<listIdApartment.count();i++){
        m_apartment.append(new Apartment(listIdApartment.at(i),this));
    }
}

void Home::rename(QString new_name)
{
    BD::UpdateTable("homes","name",new_name,"id_homes",QString::number(m_id));
}
