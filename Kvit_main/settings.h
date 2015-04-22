#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include "bd.h"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    static QSqlQueryModel *Model();
    static QSqlError Delete(QString name_setting);
    static QString Value(QString NameSetting);
    static bool createNew();

signals:

public slots:
};

#endif // SETTINGS_H
