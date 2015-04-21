#include "indications.h"

Indications::Indications(int idService, QObject *parent) : QObject(parent)
{
    m_idService = idService;
}

int Indications::valueIndicationHome(int idApartment, QDate date)
{
    QString str;
    QString out;
    DateOfUnixFormat unix_date(date.addMonths(1));
    Apartment apartment(idApartment);

    str = "SELECT pokazanie_home FROM pokazanie WHERE id_list_app_usluga=%1 AND date_pokazanie=%2 ";
    str = str.arg(apartment.isIdListApartamentServise(m_idService))
            .arg(unix_date.Second());
    BD::SelectFromTable(str,&out);

    return out.toInt();
}

Indications::~Indications()
{

}

