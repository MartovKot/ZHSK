#ifndef INDICATIONS_H
#define INDICATIONS_H

#include <QObject>
#include "bd.h"
#include "dateofunixformat.h"
#include "apartment.h"

class Indications : public QObject
{
    Q_OBJECT
public:
    explicit Indications(int idService, QObject *parent = 0);
    int valueIndicationHome(int idApartment, QDate date);
    ~Indications();
private:
    int m_idService;

signals:

public slots:
};

#endif // INDICATIONS_H
