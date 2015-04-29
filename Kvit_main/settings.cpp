#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent)
{

}

Settings::~Settings()
{

}

QSqlQueryModel *Settings::Model()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(QSqlQuery("SELECT name_setting,value_setting FROM settings"));
    model->setHeaderData(0,Qt::Horizontal,QObject::trUtf8("Название"));
    model->setHeaderData(1,Qt::Horizontal,QObject::trUtf8("Значение"));
    return model;
}

QSqlError Settings::Delete(QString name_setting)
{
    QString str;
    QSqlQuery query;
    QSqlError out;

    str = "DELETE FROM settings WHERE name_setting = '%1'";
    str = str.arg(name_setting);
    if (!query.exec(str)){
        out = query.lastError();
    }
    return out;
}

QString Settings::Value(QString NameSetting)
{
    QString out = "";
    QString str;
    QSqlQuery query;

    str = "SELECT value_setting FROM settings WHERE name_setting = '%1'";
    str = str.arg(NameSetting);

    if (query.exec(str)){
        if (query.next()){
            out = query.value(0).toString();
        }else{
            QStringList column, value;
            column << "name_setting" << "value_setting";
            value << NameSetting << "";
            BD::add("settings", column, value);
        }
    }

    return  out;
}

bool Settings::createNew()
{
    QStringList column, value;
    column << "name_setting" << "value_setting";
    value << "" << "";
    if( BD::add("settings", column, value).number() != 0){
        return false;
    }
    return true;
}

