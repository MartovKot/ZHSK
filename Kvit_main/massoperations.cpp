#include "massoperations.h"
#include "ui_massoperations.h"
#include "home.h"
#include "organization.h"
#include <QProgressDialog>

MassOperations::MassOperations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MassOperations)
{
    ui->setupUi(this);
    setWindowTitle("Услуги");
    ui->cmBx_Usluga->setModel(db.Model("usluga"));

}

MassOperations::~MassOperations()
{
    delete ui;
}

void MassOperations::on_pBtn_Add_clicked()
{
    int id_usluga;
    QList<int> apartaments;
    QStringList column, value;
    column << "id_apartament" << "id_usluga";

    id_usluga = ui->cmBx_Usluga->model()->index(ui->cmBx_Usluga->currentIndex(),1).data().toInt();
    apartaments = db.is_ListIdApartament(m_id_org,m_id_home);
    QProgressDialog progress(trUtf8("Идёт распределение..."), trUtf8("Отмена"), 0, apartaments.count()-1, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Расчёт"));
    progress.show();
    for (int i=0;i<apartaments.count();i++){
        progress.setValue(i);
        qApp->processEvents();
        if (progress.wasCanceled())
            break;
        value.clear();
        value << QString::number(apartaments.at(i)) << QString::number(id_usluga);
        db.add("list_app_usluga",column,value,6);
    }
    progress.setValue(apartaments.count()-1);
    QMessageBox::information(this,trUtf8("Успех"),
                         trUtf8("Услуга распределена всем \n"),QMessageBox::Ok);
}

void MassOperations::on_pBtn_Delete_clicked()
{
    int id_usluga;
    QList<int> apartaments;

    id_usluga = ui->cmBx_Usluga->model()->index(ui->cmBx_Usluga->currentIndex(),1).data().toInt();
    apartaments = db.is_ListIdApartament(m_id_org,m_id_home);
    for (int i=0;i<apartaments.count();i++){
       int id_lau;
       id_lau = db.is_idListAppUsluga(apartaments.at(i),id_usluga);
       db.DeleteUslugaApartament(id_lau);
    }

}

void MassOperations::setHome(int id_home)
{
    Home home;
    home.setId(id_home);
    m_id_home = id_home;
    ui->lbl_home->setText(home.getName());
}

void MassOperations::setOrganization(int id_org)
{
    Organization organization;
    organization.setId(id_org);
    m_id_org = id_org;
    ui->lbl_org->setText(organization.getName());
}
