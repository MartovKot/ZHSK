#include "apartament.h"

Apartament::Apartament()
{
}
Apartament::Apartament(int num, int id_home, int id_org)
{
    id_app = db.is_idappart(id_home,id_org,num);
    name_org =  db.is_nameOrg(id_org);
    FIO = db.is_FIO (id_app);
}
