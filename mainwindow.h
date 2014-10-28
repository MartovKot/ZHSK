#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>

#include <QImageReader>
#include <QProgressDialog>
#include <QComboBox>
#include <QKeyEvent>
#include <QCalendarWidget>

#include "viewblank.h"
#include "bd.h"
#include "table_tariff.h"
#include "editsetting.h"
#include "selecter_with_combobox.h"
#include "myitemdelegate.h"
#include "dateofunixformat.h"
#include "organization.h"
#include "apartment.h"
#include "modelusliga.h"
#include "dialog_with_lineedit.h"
#include "tableviewpokazanie.h"
#include "table_payment.h"
#include "newcounter.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setVersion(QString ver);

private:
    Ui::MainWindow *ui;
    ViewBlank *VwBlank;
    Selecter_with_ComboBox *dlgSelCmBx_org;
    Selecter_with_ComboBox *dlgSelCmBx_home;
    BD db;
    QString CurentDirArhiv;
    QString Version;

    table_tariff tbl_tariff;

    void Mode (QString m);
    struct Tarifstruct {
        int n;
        QLineEdit *le;
    };
    QList <Tarifstruct> listh;

    void Refresh_Organization();
    void Refresh_Home();
    void Refresh_Appartament();
    void Refresh_Uslugi();
    void Refresh_Pensioner();

    void Refresh_tblV_on_Uslugi();

    QList<QHBoxLayout> list;
    QDialog *dlg;
    QLineEdit *lEd_tarif;
    QLineEdit *lEd_tarif2;
    QLineEdit *lEd_norm;
    Apartment *apartment_for_apartment;

    void Refresh_tblVPayment(int id_apartment);
    void Refresh_tblVCount(int id_apartment);
    void Refresh_LabelPayAndDebt(int id_apartment);

    TableViewPokazanie *tbl;
    int HomeID;
    int OrganizationID;
    int p_id_pokazanie;
    int p_pokazanie;
    void Refresh_lbl_Payer();

private slots:
    void Admin_mod();
    void Oper_mod();
    void Print_mod();
    void sl_ArhivKvit();  //Архив квитанций
    void sl_setCurDirArh(QString CurDir);
    void sl_OpenArhiv();

    void Refresh_Settings();
    void sl_AddOrg();                                  // добавление организации
    void AddHome();                                    // добавление дома
    void sl_AddUsluga(const QString &usluga, const int &id_type_usluga);
    void Refresh(int);
    void Refresh_cmbNumApp_onUslugi();
    void Refresh_cmBx_NumApp_onApartament();
    void Refresh_tblView_Apartament();
    void TarifRequest();
    void TarifEdit();
    void SaveTarif();
    void FillTarif();
    void sl_OrgEdit(QModelIndex model);
    void sl_HomeEdit(QModelIndex model);
    void sl_SaveOrg();
    void sl_SaveHome();
    void sl_DeleteOrg();
    void sl_DeleteHome();
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
    void on_pBtn_Save_clicked();
    void on_cmBx_NumAp_on_Apartament_activated(int index);
    void on_pBtn_DeleteApartment_clicked();
    void on_cmBx_Org_on_Uslugi_activated(const QString &arg1);
    void on_cmBx_Home_on_Uslugi_activated(const QString &arg1);
    void on_cmBx_NumAp_on_Uslugi_activated(const QString &arg1);
    void on_pBtnAddUsluga_clicked();
    void on_pBtnDeleteUsluga_clicked();
    void sl_addPayment();


    void sl_EditPokazanie();
    void sl_DeletePayment();
    void sl_Calendar();

    void sl_ApartFirst();
    void sl_ApartLast();
    void sl_ApartNext();
    void sl_ApartPrevious();
    void sl_NewCounter();               //вызывается когда происходит смена счётчика
    void sl_Refresh_cmBx_NumApartanent();

    void on_pBtn_NewCounterNext_clicked();

    void on_tBtn_org_clicked();

    void on_tBtn_Home_clicked();

    void on_cmBx_NumApartanent_currentIndexChanged(const QString &arg1);
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
