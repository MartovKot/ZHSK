#ifndef INDICATIONS_H
#define INDICATIONS_H

#include <QObject>
#include "bd.h"
#include "dateofunixformat.h"
#include "apartment.h"
#include "sqlqueryeditmodel.h"
#include "TransposeProxyModel.h"
#include "dateofunixformat.h"

class Indications : public QObject
{
    Q_OBJECT
public:
    explicit Indications(int idService, int idApartment, QDate date, QObject *parent = 0);
    explicit Indications(int idIndication, QObject *parent = 0);
    explicit Indications(QObject *parent = 0);
    int valueIndicationHome();
    void UpdateHome(int newValue);
    static int New(int id_pok_old, QString value_home);
    static void New(int id_apartament, int month, int year);
    SqlQueryEditModel *ModelEditPokazanie(int id_apartament, int month, int year);      //редактируемые показания
    static QSqlQueryModel *ModelPokazanie(int id_apartament, int month, int year );            //показания
    QString getNameService();
    ~Indications();
private:
    void setDefault();
    int m_idService;
    int m_idApartment;
    int m_id;
    qint64 m_Udate;
    void calcSewerage(bool New = false);

signals:
private slots:
    void slEdit(int id_pok, QString value);
    void slModelPokazanieHeaderData(QAbstractTableModel* t);

public slots:
};

#endif // INDICATIONS_H
