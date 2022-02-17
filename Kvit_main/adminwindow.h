#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSqlError>
#include <QHeaderView>

#include "table_tariff.h"
#include "editsetting.h"
#include "selecter_with_combobox.h"
#include "myitemdelegate.h"
#include "dateofunixformat.h"
#include "apartment.h"
#include "modelusliga.h"
#include "dialog_with_lineedit.h"
#include "settings.h"
#include "modelapartament.h"

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

    QList<QHBoxLayout> list;
    QDialog *dlg;
    QLineEdit *lEd_tarif;
    QLineEdit *lEd_tarif2;
    QLineEdit *lEd_norm;

private slots:
    void Refresh_Settings();
    void Refresh_tblV_on_Uslugi();
    void sl_AddOrg();                                  // добавление организации
    void AddHome();                                    // добавление дома
    void sl_AddUsluga(const QString &usluga, const int &id_type_usluga);
    void Refresh(int);
    void Refresh_cmbNumApp_onUslugi();
    void Refresh_cmBx_NumApp_onApartament();
    void Refresh_tblView_Apartament();
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
    void on_pBtn_Save_clicked();
    void on_cmBx_NumAp_on_Apartament_activated(int index);
    void on_pBtn_DeleteApartment_clicked();
    void on_cmBx_Org_on_Uslugi_activated(const QString &arg1);
    void on_cmBx_Home_on_Uslugi_activated(const QString &arg1);
    void on_cmBx_NumAp_on_Uslugi_activated(const QString &arg1);
    void on_pBtnAddUsluga_clicked();
    void on_pBtnDeleteUsluga_clicked();
    void on_pBtnMassOperation_clicked();
    void on_pBtn_Tarif_request_clicked();
    void on_cmBx_PensApart_currentTextChanged(const QString &arg1);
};

#endif // ADMINWINDOW_H
