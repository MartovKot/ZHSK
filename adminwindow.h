#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QtGui>
#include <QMessageBox>
#include <QSpacerItem>

#include"bd.h"

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

    void Mode (QString m);
    struct Tarifstruct {
        int n;
        QLineEdit *le;
    };
    QList <Tarifstruct> listh;

    void Refresh_Organization();
    void Refresh_Home();
    void Refresh_Appartament(int id_apartament=-1);
    void Refresh_Uslugi();
    QList<QHBoxLayout> list;
    QDialog *dlg;
    QLineEdit *lEd_tarif;
    QLineEdit *lEd_tarif2;
    QLineEdit *lEd_norm;
    int ID_ORG;
    int ID_HOME;
private slots:
    void AddOrg();                                  // добавление организации
    void AddHome();                                 // добавление дома
    void AddUsluga_of_Apartament();                 // Добавить услуги в квартиру
    void Refresh();
    void Refresh_cmbNumApp_onUslugi();
    bool Verify();
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
};

#endif // ADMINWINDOW_H
