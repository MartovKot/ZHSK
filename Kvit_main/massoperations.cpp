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
    ui->cmBx_Usluga->setModel(Service::modelService());

}

MassOperations::~MassOperations()
{
    delete ui;
}

void MassOperations::on_pBtn_Add_clicked()
{

    QStringList column, value;
    column << "id_apartament" << "id_usluga";

    Service service(ui->cmBx_Usluga->currentText());

    Home home(m_id_home);

    QProgressDialog progress(trUtf8("Идёт распределение..."), trUtf8("Отмена"), 0, home.apartments().count()-1, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Расчёт"));
    progress.show();
    for (int i=0;i<home.apartments().count();i++){
        progress.setValue(i);
        qApp->processEvents();
        if (progress.wasCanceled())
            break;
        value.clear();
        value << QString::number(home.apartments().at(i)->getId()) << QString::number(service.getIdService());
        db.add("list_app_usluga",column,value,6);
    }
    progress.setValue(home.apartments().count()-1);
    QMessageBox::information(this,trUtf8("Успех"),
                         trUtf8("Услуга распределена всем \n"),QMessageBox::Ok);
    emit end();
}

void MassOperations::on_pBtn_Delete_clicked()
{
    Home home(m_id_home);
    for (int i=0;i<home.apartments().count();i++){
        home.apartments().at(i)->deleteService(ui->cmBx_Usluga->currentText());
    }
}

void MassOperations::setHome(int id_home)
{
    Home home(id_home);
    m_id_home = id_home;
    ui->lbl_home->setText(home.getName());
}

void MassOperations::setOrganization(int id_org)
{
    Organization organization(id_org);
    m_id_org = id_org;
    ui->lbl_org->setText(organization.name());
}
