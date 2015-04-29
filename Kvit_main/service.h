#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include "bd.h"

class Service : public QObject
{
    Q_OBJECT
public:
    Service(int idService);
    Service(QString name);
    ~Service();
    int getIdType() const;
    void setIdType(int value);

    QString getName() const;
    void setName(const QString &value);

    bool isElectro();
    bool isElectroDay();
    bool isElectroNight();
    static QSqlQueryModel *modelServiceType();
    static QSqlQueryModel *modelService();
    static bool createNew(const QString &name,const int idType);
    bool remove();

    int getIdService() const;
    void setIdService(int value);

private:
    int idService;
    int idType;
    QString name;
    BD db;
};

#endif // SERVICE_H
