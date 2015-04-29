#ifndef EDITINDICATIONSCOUNTER_H
#define EDITINDICATIONSCOUNTER_H

#include <QDialog>
#include "tableviewpokazanie.h"
#include "myitemdelegate.h"
#include "sqlqueryeditmodel.h"
#include "indications.h"

namespace Ui {
class EditIndicationsCounter;
}

class EditIndicationsCounter : public QDialog
{
    Q_OBJECT

public:
    explicit EditIndicationsCounter(int idApartment, int month, int year, QWidget *parent = 0);
    ~EditIndicationsCounter();

private:
    Ui::EditIndicationsCounter *ui;

    TableViewPokazanie *tbl;
    MyItemDelegate *dDeleg;
    SqlQueryEditModel *edModel;
    int idApartament;
    int month;
    int year;
private slots:
    void slEdit(int id_pok, QString value);
    void slModelPokazanieHeaderData(QAbstractTableModel* t);
};

#endif // EDITINDICATIONSCOUNTER_H
