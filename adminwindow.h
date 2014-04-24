#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSqlError>
#include <QHeaderView>

#include "bd.h"
#include "table_tariff.h"
#include "editsetting.h"
#include "selecter_with_combobox.h"
#include "myitemdelegate.h"

namespace Ui {
    class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = 0);
    ~AdminWindow();

private:
    Ui::AdminWindow *ui;
    BD db;
    table_tariff tbl_tariff;

    void Mode (QString m);
    struct Tarifstruct {
        int n;
        QLineEdit *le;
    };
    QList <Tarifstruct> listh;

    void Refresh_Organization();
    void Refresh_Home();
    void Refresh_Appartament(/*int id_apartament=-1*/);
    void Refresh_Uslugi();
    void Refresh_Pensioner();



    QList<QHBoxLayout> list;
    QDialog *dlg;
    QLineEdit *lEd_tarif;
    QLineEdit *lEd_tarif2;
    QLineEdit *lEd_norm;

    qint64 IsDateOfUnix(int year, int month, int day)
    {
        qint64 timeInUnix;
        const qint64 MS_COEF = 1000;
        QDate date;
        date.setDate(year, month, day);
        QDateTime datetime;
        datetime.setTimeSpec(Qt::OffsetFromUTC);
        datetime.setDate(date);
        timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;
        return timeInUnix;
    }

//    int isIdSelectHome_onApartament();
//    int isIdSelectOrganiztion_onApartament();
private slots:
    void Refresh_Settings();
    void AddOrg();                                  // добавление организации
    void AddHome();                                 // добавление дома
    void AddUsluga_of_Apartament();                 // Добавить услуги в квартиру
    void Refresh(int);
    void Refresh_cmbNumApp_onUslugi();
    void Refresh_cmBx_NumApp_onApartament();
    void Refresh_tblView_Apartament();
    void SaveApart();
    void TarifRequest();
    void TarifEdit();
    void SaveTarif();
    void FillTarif();
    void UslView();
    void sl_OrgEdit(QModelIndex model);
    void sl_HomeEdit(QModelIndex model);
    void sl_SaveOrg();
    void sl_SaveHome();
    void sl_DeleteOrg();
    void sl_DeleteHome();
    void sl_DeleteUsluga();
    void sl_CancelOrg();
    void sl_CancelHome();
    void on_pBtn_addPens_clicked();
    void on_pBtn_delPens_clicked();
    void on_cmBx_PensApart_activated(const QString &arg1);
    void on_tblV_on_Pens_clicked(const QModelIndex &index);
    void on_pBtn_NewApartament_clicked();
    void on_pBtn_Cancel_onApart_clicked();
    void on_tBtn_AddSetting_clicked();
    void on_tBtn_DeleteSetting_clicked();
    void on_tBtn_EditSetting_clicked();
    void on_pBtn_OrgSelect_clicked();
    void on_pBtn_HomeSelect_clicked();
    void sl_SelectHome(QString home_name);
    void sl_SelectOrg(QString org_name);
};

#endif // ADMINWINDOW_H
