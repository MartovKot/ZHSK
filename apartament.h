#ifndef APARTAMENT_H
#define APARTAMENT_H

#include <QString>
#include "bd.h"

class Apartament
{
private:
    int id_app;
    BD db;
public:
    Apartament();
    Apartament(int num, int id_home, int id_org);
    QString name_org;
    QString FIO;
};

#endif // APARTAMENT_H
