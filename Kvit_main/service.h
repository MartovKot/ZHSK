#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include "bd.h"

class Service : public QObject
{
    Q_OBJECT
public:
    Service(int idService);
    ~Service();
    int getIdType() const;
    void setIdType(int value);

    QString getName() const;
    void setName(const QString &value);

private:
    int idService;
    int idType;
    QString name;
    BD db;
};

#endif // SERVICE_H
