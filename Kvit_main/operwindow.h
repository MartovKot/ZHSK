#ifndef OPERWINDOW_H
#define OPERWINDOW_H

#include <QDialog>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QBitmap>
#include <QKeyEvent>
#include <QCalendarWidget>
#include <QHBoxLayout>

#include "bd.h"
#include "tableviewpokazanie.h"
#include "newcounter.h"
#include "table_payment.h"

#include "organization.h"
#include "home.h"
#include "apartment.h"
#include "calculation.h"

namespace Ui {
    class OperWindow;
}

class OperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OperWindow(QWidget *parent = 0);
    ~OperWindow();
    void set_parametr(int id_org, int id_home);

private:
    Ui::OperWindow *ui;
    BD db;
    Calculation calculation;
    void Refresh_tblVPayment(int ApartamentID);
    TableViewPokazanie *tbl;
    QDialog *dlg;
    int HomeID;
    int OrganizationID;
    int p_id_pokazanie;
    int p_pokazanie;
    void Refresh_lbl_Payer();

private slots:
    void sl_addPayment();
    void Refresh_tblVCount();
    void sl_RefreshFull();
    void sl_EditPokazanie();
    void sl_DeletePayment();
    void sl_Calendar();
    void sl_RefreshLabel();
    void sl_ApartFirst();
    void sl_ApartLast();
    void sl_ApartNext();
    void sl_ApartPrevious();
    void sl_NewCounter();               //вызывается когда происходит смена счётчика

    void on_pBtn_NewCounterNext_clicked();

    void on_pBtn_AddPayment_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif // OPERWINDOW_H
