#ifndef HOME_H
#define HOME_H

#include <QObject>
#include "bd.h"

class Home : public QObject
{
    Q_OBJECT
public:
    explicit Home();
    void setId(int id);
    QString getName();
private:
    QString m_name;
    int m_id;
    BD db;

signals:

public slots:

};

#endif // HOME_H
