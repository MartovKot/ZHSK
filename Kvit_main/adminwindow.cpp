#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "mainwindow.h"
#include "massoperations.h"


#include <qtextcodec.h>

#include <QStandardItemModel>
#include <QCheckBox>
#include <QTableWidgetItem>



AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    QRegExpValidator *only_number;
//    QRegExpValidator *dv;
    only_number = new QRegExpValidator(QRegExp("[0-9]+"),this);                 //валидатор для цифр
//    dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),this);       //валидатор для чисел

    setLayout(ui->gridLayout);                                                  //главный слой

    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),SLOT(Refresh(int)));
    connect(this,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    ui->tabWidget->setCurrentIndex(0);

    // настройка вкладки Организации
    ui->tab_Organization->setLayout(ui->verticalLayout);                        // вкладка Организации
    ui->lEdAcc->setValidator(only_number);                                      // поле с лицевым счётом
    ui->lEdINN->setValidator(only_number);
    connect(ui->pBtn_addOrg,SIGNAL(clicked()),SLOT(sl_AddOrg()));
    connect(ui->tblView_Organization,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_OrgEdit(QModelIndex)));
    connect(ui->pBtn_SaveOrg,SIGNAL(clicked()),SLOT(sl_SaveOrg()));
    connect(ui->pBtn_DeleteOrg,SIGNAL(clicked()),SLOT(sl_DeleteOrg()));
    connect(ui->pBtn_CancelOrg,SIGNAL(clicked()),SLOT(sl_CancelOrg()));

    Mode("org_default");
    Refresh_Organization(); // Делаем обновление тк сюда мы приходим сразу
    // -------------------------------

    // настройка вкладки Дома
    ui->tab_Home->setLayout(ui->verticalLayout_3);                              // вкладка дома
    Mode("home_default");
    connect(ui->pBtn_addHome,SIGNAL(clicked()),SLOT(AddHome()));                // добавление дома
    connect(ui->tblView_Home,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_HomeEdit(QModelIndex)));
    connect(ui->pBtn_CancelHome,SIGNAL(clicked()),SLOT(sl_CancelHome()));
    connect(ui->pBtn_DeleteHome,SIGNAL(clicked()),SLOT(sl_DeleteHome()));
    connect(ui->pBtn_SaveHome,SIGNAL(clicked()),SLOT(sl_SaveHome()));

    //------------------------------------

    // настройка Квартиры
    apartment_for_apartment = nullptr;
    ui->lbl_HomeSelect->setText("");
    ui->lbl_OrgSelect->setText("");
    ui->tab_Appartament->setLayout(ui->verticalLayout_2);                       //Вкладка квартиры
    ui->lEd_NumApart_onApart->setValidator(only_number);

    Mode("app_deff");
    //------------------------------------


    //настройка Услуги
    ui->tab_Uslugi->setLayout(ui->verticalLayout_9);
    //------------------------------------

    //настройка тарифы
    ui->lEd_Tarif_year->setText(QString::number(QDate::currentDate().year()));
    ui->lEd_Tarif_month->setText(QString::number(QDate::currentDate().month()));
    ui->tab_Tarif->setLayout(ui->verticalLayout_8);

    AdminWindow::connect(ui->pBtn_TarifEdit,SIGNAL(clicked()),SLOT(TarifEdit()));
    AdminWindow::connect(ui->pBtn_Fill,SIGNAL(clicked()),SLOT(FillTarif()));
    //---------------------------------------

    //настройка пенсионеры


    ui->tab_Pensioner->setLayout(ui->mainLay_Pens);
    Refresh_Pensioner();
    //----------------------------------------

    //настройка настройки
    ui->tab_Settings->setLayout(ui->vLay_Settings);
    Refresh_Settings();


}

AdminWindow::~AdminWindow()
{
    delete apartment_for_apartment;
    delete ui;
}

void AdminWindow::sl_AddOrg()                                                  //Добавление организации
{
    Organization organization;

    if (!organization.New(ui->lEdNameOrg->text(), ui->lEdBank->text(), ui->lEdAcc->text(), ui->lEdINN->text())){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Организация не добавлена"),QMessageBox::Ok);
        return;
    }

    ui->lEdAcc->setText("");
    ui->lEdBank->setText("");
    ui->lEdNameOrg->setText("");
    Refresh_Organization();
}

void AdminWindow::AddHome()
{
    QStringList column, value;
    column<<"name";
    value<<ui->lEdHome->text();
    if (db.add("homes",column,value) != 0){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Запись не создана"),QMessageBox::Ok);
        return;
    }
    ui->lEdHome->setText("");
    Refresh_Home();
}


void AdminWindow::Refresh_Appartament()
{
    if (ui->lbl_HomeSelect->text() == ""){
        ui->lbl_HomeSelect->setText(trUtf8("<font color = red>Дом не выбран</font>"));
    }
    if ( ui->lbl_OrgSelect->text() == ""){
        ui->lbl_OrgSelect->setText(trUtf8("<font color = red>Организация не выбрана</font>"));
    }
    Refresh_cmBx_NumApp_onApartament();
    Refresh_tblView_Apartament();
}

void AdminWindow::Refresh_tblView_Apartament()
{
    MyItemDelegate * dDeleg = new MyItemDelegate ();
    Home home;
    home.setName(ui->lbl_HomeSelect->text());
    Organization organization;
    organization.setName(ui->lbl_OrgSelect->text());

    if (apartment_for_apartment == nullptr){
        apartment_for_apartment = new Apartment(home.getId(),
                                                organization.getId() ,
                                                ui->cmBx_NumAp_on_Apartament->currentText().toInt());
    }

    TransposeProxyModel *trans = new TransposeProxyModel;
    trans->setSourceModel(apartment_for_apartment->ModelOneApartment(apartment_for_apartment->getId()));
    ui->tblView_Apartament->setModel(trans);
    connect(apartment_for_apartment,SIGNAL(sgn_EditModel()),SLOT(Refresh_tblView_Apartament()));


    ui->tblView_Apartament->setItemDelegate(dDeleg);

    ui->tblV_on_Uslugi->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AdminWindow::Mode(QString m)
{
    int i;
    if (m=="app_deff")
        i=1;
    if (m=="app_edit")
        i=2;
    if (m=="app_add")
        i=3;
    if (m == "usl_def")
        i = 4;
    if (m == "usl_add")
        i = 5;
    if (m == "org_default")
        i = 6;
    if (m == "org_edit")
        i = 7;
    if (m == "home_default")
        i = 8;
    if (m == "home_edit")
        i = 9;
    switch(i){
    case  1: //app_deff
        ui->pBtn_OrgSelect->setEnabled(1);
        ui->pBtn_HomeSelect->setEnabled(1);
        ui->pBtn_NewApartament->setEnabled(1);

        ui->lEd_NumApart_onApart->setHidden(true);
        ui->cmBx_NumAp_on_Apartament->setHidden(false);
        ui->pBtn_Cancel_onApart->setHidden(true);
        ui->pBtn_Save->setHidden(true);
        ui->tblView_Apartament->setHidden(false);

        break;
    case 2: //app_edit
        ui->pBtn_OrgSelect->setEnabled(0);
        ui->pBtn_HomeSelect->setEnabled(0);
        ui->pBtn_Save->setEnabled(1);
        break;
    case 3: //app_add
        ui->pBtn_OrgSelect->setEnabled(0);
        ui->pBtn_HomeSelect->setEnabled(0);
        ui->pBtn_NewApartament->setEnabled(0);

        ui->lEd_NumApart_onApart->setHidden(false);
        ui->cmBx_NumAp_on_Apartament->setHidden(true);
        ui->pBtn_Cancel_onApart->setHidden(false);
        ui->pBtn_Save->setHidden(false);
        ui->tblView_Apartament->setHidden(true);

        break;
    case 4:
//        ui->tblV_on_Uslugi->setEnabled(0);
//        ui->pBtn_AddUsluga->setEnabled(0);
//        ui->cmBx_Uslugi_onUslugi->setEnabled(0);
        break;
    case 5:
//        ui->tblV_on_Uslugi->setEnabled(1);
//        ui->pBtn_AddUsluga->setEnabled(1);
//        ui->cmBx_Uslugi_onUslugi->setEnabled(1);
        break;
    case 6: //org_default
        ui->pBtn_SaveOrg->setHidden(true);                                          //скрытие кнопки
        ui->pBtn_DeleteOrg->setHidden(true);
        ui->pBtn_CancelOrg->setHidden(true);
        ui->lEdNameOrg->setText("");                                                // очистка формы
        ui->lEdBank->setText("");
        ui->lEdAcc->setText("");
        ui->lEdINN->setText("");
        ui->tblView_Organization->setEnabled(true);
        ui->pBtn_addOrg->setHidden(false);

        ui->pBtn_SaveOrg->setHidden(true);
        ui->pBtn_DeleteOrg->setHidden(true);
        ui->pBtn_CancelOrg->setHidden(true);

        break;
    case 7: //org_edit
        ui->pBtn_SaveOrg->setHidden(false);                                          //скрытие кнопки
        ui->pBtn_DeleteOrg->setHidden(false);
        ui->pBtn_CancelOrg->setHidden(false);
        ui->tblView_Organization->setEnabled(false);
        ui->pBtn_addOrg->setHidden(true);
        break;
    case 8: //home_default
        ui->pBtn_SaveHome->setHidden(true);
        ui->pBtn_DeleteHome->setHidden(true);
        ui->pBtn_CancelHome->setHidden(true);
        ui->lEdHome->setText("");
        ui->pBtn_addHome->setHidden(false);
        ui->tblView_Home->setEnabled(true);
        break;
    case 9: //home_edit
        ui->pBtn_SaveHome->setHidden(false);
        ui->pBtn_DeleteHome->setHidden(false);
        ui->pBtn_CancelHome->setHidden(false);
        ui->pBtn_addHome->setHidden(true);
        ui->tblView_Home->setEnabled(false);
        break;
    }
}



void AdminWindow::SaveTarif()
{
    int TarifID = ui->tblV_on_Tarif->model()->index(ui->tblV_on_Tarif->currentIndex().row(),0).data().toInt();

    tbl_tariff.UpdateTarif(lEd_tarif->text(),lEd_tarif2->text(),lEd_norm->text(),TarifID);
    on_pBtn_Tarif_request_clicked();
    dlg->close();
    delete dlg;
}

void AdminWindow::TarifEdit()
{
    if (!ui->tblV_on_Tarif->currentIndex().isValid()){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не выбран тариф"),QMessageBox::Ok);
        return;
    }

    QRegExpValidator *dv;
    dv = new QRegExpValidator(QRegExp("[0-9]+\\.?[0-9]{,2}"),this); //валидатор для чисел
    dlg = new QDialog(this);
    dlg->setPalette(this->palette());
    QVBoxLayout *main_lay = new QVBoxLayout;
    QHBoxLayout *lay_1 = new QHBoxLayout;
    QHBoxLayout *lay_2 = new QHBoxLayout;
    QHBoxLayout *lay_3 = new QHBoxLayout;
    QVBoxLayout *lay_4 = new QVBoxLayout;
    QVBoxLayout *lay_5 = new QVBoxLayout;
    QVBoxLayout *lay_6 = new QVBoxLayout;

    QLabel *lblUp = new QLabel;
    lblUp->setText("<H1><font color=grey >"+ui->lEd_Tarif_month->text()
                   + "  "+ui->lEd_Tarif_year->text()+"</font></H1>");
    lay_1->addWidget(lblUp);

    QLabel *lblUsluga = new QLabel;
    lblUsluga->setText(ui->tblV_on_Tarif->model()->index(
                           ui->tblV_on_Tarif->currentIndex().row(),1).data().toString());
    lay_2->addWidget(lblUsluga);

    lEd_tarif = new QLineEdit;
    lEd_tarif->setValidator(dv);
    lEd_tarif->setText(ui->tblV_on_Tarif->model()->index(
                           ui->tblV_on_Tarif->currentIndex().row(),2).data().toString());
    lEd_tarif2 = new QLineEdit;
    lEd_tarif2->setValidator(dv);
    lEd_tarif2->setText(ui->tblV_on_Tarif->model()->index(
                            ui->tblV_on_Tarif->currentIndex().row(),3).data().toString());
    lEd_norm = new QLineEdit;
    lEd_norm->setValidator(dv);
    lEd_norm->setText(ui->tblV_on_Tarif->model()->index(
                          ui->tblV_on_Tarif->currentIndex().row(),4).data().toString());

    lay_4->addWidget(new QLabel(QObject::trUtf8("Тариф")));
    lay_4->addWidget(lEd_tarif);
    lay_5->addWidget(new QLabel(QObject::trUtf8("Тариф2")));
    lay_5->addWidget(lEd_tarif2);
    lay_6->addWidget(new QLabel(QObject::trUtf8("Норма")));
    lay_6->addWidget(lEd_norm);
    lay_2->addLayout(lay_4);
    lay_2->addLayout(lay_5);
    lay_2->addLayout(lay_6);

    QPushButton *pBtn_TarifSave = new QPushButton(QObject::trUtf8("Сохранить"));
    lay_3->addWidget(pBtn_TarifSave);
    AdminWindow::connect(pBtn_TarifSave,SIGNAL(clicked()),SLOT(SaveTarif()));

    main_lay->addLayout(lay_1);
    main_lay->addLayout(lay_2);

    main_lay->addLayout(lay_3);
    dlg->setLayout(main_lay);
    dlg->setModal(true);
    dlg->show();
}

void AdminWindow::Refresh_Organization()
{
    Organization organization;
    delete ui->tblView_Organization->model();
    ui->tblView_Organization->setModel(organization.ModelAllOrganization());
    ui->tblView_Organization->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
}

void AdminWindow::Refresh_Home()
{
    Home home;
    delete ui->tblView_Home->model();
    ui->tblView_Home->setModel(home.ModelAllHome());
    ui->tblView_Home->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
}

void AdminWindow::Refresh_Uslugi()
{
    Home home;
    ui->cmBx_Home_on_Uslugi->setModel(home.ModelAllHome());
    ui->cmBx_Home_on_Uslugi->setModelColumn(1);

    Organization organization;
    ui->cmBx_Org_on_Uslugi->setModel(organization.ModelAllOrganization());
    ui->cmBx_Org_on_Uslugi->setModelColumn(1);


    Refresh_cmbNumApp_onUslugi();
    Refresh_tblV_on_Uslugi();

    Mode("usl_def");

}

void AdminWindow::Refresh_Pensioner()
{
    Apartment apartment;
    ui->cmBx_PensApart->setModel(
    apartment.ModelAllApartment(ui->cmBx_Home_on_Pens->model()->index(ui->cmBx_Home_on_Pens->currentIndex(),1).data().toInt()
                       ,ui->cmBx_Org_on_Pens->model()->index(ui->cmBx_Org_on_Pens->currentIndex(),1).data().toInt()));
    ui->cmBx_PensApart->addItem("");
    ui->cmBx_Org_on_Pens->setModel(db.Model("organiz"));
    ui->cmBx_Home_on_Pens->setModel(db.Model("homes"));
    ui->tblV_on_Pens->setModel(db.ModelPensioner(ui->cmBx_Home_on_Pens->model()->index(
                                                     ui->cmBx_Home_on_Pens->currentIndex(),1).data().toInt()
                                                 ,ui->cmBx_Org_on_Pens->model()->index(
                                                     ui->cmBx_Org_on_Pens->currentIndex(),1).data().toInt()));
    ui->tblV_on_Pens->hideColumn(0);
    ui->tblV_on_Pens->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
}

void AdminWindow::Refresh_cmbNumApp_onUslugi()
{
    int OrganiztionID = -1, HomeID = -1;
    int row;

    row = ui->cmBx_Org_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                OrganiztionID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле организация или дома"),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле организация или дома"),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле организация или дома"),QMessageBox::Ok);
        return;
    }

    row = ui->cmBx_Home_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                HomeID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
        return;
    }

    QSqlQueryModel *model;
    Apartment apartment;
    model = apartment.ModelAllApartment(HomeID,OrganiztionID);
    ui->cmBx_NumAp_on_Uslugi->setModel(model);

}


void AdminWindow::Refresh_cmBx_NumApp_onApartament()
{
    Organization organization;
    organization.setName(ui->lbl_OrgSelect->text());

    Home home;
    home.setName(ui->lbl_HomeSelect->text());

    Apartment apartment;
    ui->cmBx_NumAp_on_Apartament->setModel(apartment.ModelAllApartment(home.getId(),organization.getId()));

    Mode("app_def");
}

void AdminWindow::Refresh(int num_tab)
{
    switch (num_tab) {
    case 0:
        Refresh_Organization();
        break;
    case 1:
        Refresh_Home();
        break;
    case 2:
        Refresh_Appartament();
        break;
    case 3:
        Refresh_Uslugi();
        break;
    case 4:
        Refresh_Pensioner();
        break;
    case 5:
        Refresh_Settings();
        break;
    default:
        break;
    }
}

void AdminWindow::FillTarif()
{
    tbl_tariff.FillTarif(ui->lEd_Tarif_month->text().toInt(),ui->lEd_Tarif_year->text().toInt());
    QMessageBox::information(this,trUtf8("Уведомление"),
                             trUtf8("Загрузка завершена"),QMessageBox::Ok);
    on_pBtn_Tarif_request_clicked();
}

void AdminWindow::sl_OrgEdit(QModelIndex model)
{
    ui->lEdNameOrg->setText(ui->tblView_Organization->model()->index(model.row(),1).data().toString());
    ui->lEdBank->setText(ui->tblView_Organization->model()->index(model.row(),2).data().toString());
    ui->lEdAcc->setText(ui->tblView_Organization->model()->index(model.row(),3).data().toString());
    ui->lEdINN->setText(ui->tblView_Organization->model()->index(model.row(),4).data().toString());
    Mode("org_edit");

}

void AdminWindow::sl_HomeEdit(QModelIndex model)
{
    ui->lEdHome->setText(ui->tblView_Home->model()->index(model.row(),1).data().toString());
    Mode("home_edit");
}

void AdminWindow::sl_SaveOrg()
{
    //обновление записи
    Organization organization(ui->tblView_Organization->model()->index(
                                  ui->tblView_Organization->currentIndex().row(), 0 ).data().toInt());

    organization.Update(ui->lEdNameOrg->text(),
                        ui->lEdBank->text(),
                        ui->lEdAcc->text(),
                        ui->lEdINN->text());

    Refresh_Organization(); //обновление таблицы
    Mode("org_default");
}
void AdminWindow::sl_DeleteOrg()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    Organization organization;
    organization.setName(ui->lEdNameOrg->text());
    organization.deleteFromDB();
    Mode("org_default");
    Refresh_Organization();                                                     //обновление таблицы
}

void AdminWindow::sl_CancelOrg()
{
    Mode("org_default");
}

void AdminWindow::sl_CancelHome()
{
    Mode("home_default");
}

void AdminWindow::sl_DeleteHome()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    Home home;
    home.setName(ui->lEdHome->text());
    home.deleteFromDB();
    Mode("home_default");
    Refresh_Home();                                                     //обновление таблицы
}

void AdminWindow::sl_SaveHome()
{
    Home home;
    home.setId(ui->tblView_Home->model()->index(
                   ui->tblView_Home->currentIndex().row(),0).data().toInt());
    home.rename(ui->lEdHome->text());
//    db.UpdateHome(ui->tblView_Home->model()->index(
//                      ui->tblView_Home->currentIndex().row(),0).data().toInt(),ui->lEdHome->text());
    Refresh_Home();
    Mode("home_default");
}

void AdminWindow::on_pBtn_addPens_clicked()
{
    QString id_apart;
    id_apart = ui->cmBx_PensApart->model()->index(ui->cmBx_PensApart->currentIndex(),1).data().toString();
    db.add("pensioner_living_alone","id_apartament",id_apart);
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(false);
    Refresh_Pensioner();
}

void AdminWindow::on_pBtn_delPens_clicked()
{
    db.DeletePension(ui->tblV_on_Pens->model()->index(ui->tblV_on_Pens->currentIndex().row(),0).data().toInt());
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(false);
    Refresh_Pensioner();
}

void AdminWindow::on_cmBx_PensApart_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->pBtn_addPens->setEnabled(true);
    ui->pBtn_delPens->setEnabled(false);
}

void AdminWindow::on_tblV_on_Pens_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(true);
}

void AdminWindow::on_pBtn_NewApartament_clicked()
{
    int id_org;
    int id_home;
    Organization organization;
    organization.setName(ui->lbl_OrgSelect->text());

    id_org = organization.getId();
    if(id_org == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    Home home;
    home.setName(ui->lbl_HomeSelect->text());
    id_home = home.getId();
    if(id_home == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    Mode("app_add");

    ui->lEd_NumApart_onApart->setText("");
}

void AdminWindow::on_pBtn_Cancel_onApart_clicked()
{
    Mode("app_deff");
}

void AdminWindow::Refresh_Settings()
{
    ui->tblV_on_Settings->setModel(db.ModelSettings());

    ui->tblV_on_Settings->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Settings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void AdminWindow::on_tBtn_AddSetting_clicked()
{
    QStringList column, value;
    column << "name_setting" << "value_setting";
    value << "" << "";
    db.add("settings", column, value);
    Refresh_Settings();
}

void AdminWindow::on_tBtn_DeleteSetting_clicked()
{
    db.DeleteSetting(ui->tblV_on_Settings->model()->index(ui->tblV_on_Settings->currentIndex().row(),0).data().toString());
    Refresh_Settings();
}

void AdminWindow::on_tBtn_EditSetting_clicked()
{
    QString name_setting;

    if (ui->tblV_on_Settings->currentIndex().row() == -1){
        return;
    }

    name_setting = ui->tblV_on_Settings->model()->index(ui->tblV_on_Settings->currentIndex().row(),0).data().toString();
    EditSetting *dlg = new EditSetting(this);
    dlg->setNameSetting(name_setting);
    connect(dlg,SIGNAL(s_Ok()),this,SLOT(Refresh_Settings()));
    dlg->open();
}

void AdminWindow::on_pBtn_OrgSelect_clicked()
{
    Selecter_with_ComboBox *dlg = new Selecter_with_ComboBox(this);
    connect(dlg,SIGNAL(CurrentValue(QString)),SLOT(sl_SelectOrg(QString)));
    connect(dlg,SIGNAL(CurrentValue(QString)),dlg,SLOT(close()));
    dlg->setWindowTitle(trUtf8("Выбор организации"));
    dlg->setContentsComboBox(db.Model("organiz"));
    dlg->open();
}

void AdminWindow::on_pBtn_HomeSelect_clicked()
{
    Selecter_with_ComboBox *dlg = new Selecter_with_ComboBox(this);
    connect(dlg,SIGNAL(CurrentValue(QString)),SLOT(sl_SelectHome(QString)));
    connect(dlg,SIGNAL(CurrentValue(QString)),dlg,SLOT(close()));
    dlg->setWindowTitle(trUtf8("Выбор дома"));
    dlg->setContentsComboBox(db.Model("homes"));
    dlg->open();
}

void AdminWindow::sl_SelectHome(QString home_name)
{
    Home home;
    home.setName(home_name);

    if (home.getId() != -1){
        ui->lbl_HomeSelect->setText(home_name);
    }else{
        ui->lbl_HomeSelect->setText(trUtf8("<font color = red>Дом не выбран</font>"));
    }
    Refresh_cmBx_NumApp_onApartament();
    delete ui->tblView_Apartament->model();
    Refresh_tblView_Apartament();
}

void AdminWindow::sl_SelectOrg(QString org_name)
{
    Organization organization;
    organization.setName(org_name);
    if (organization.getId() != -1){
        ui->lbl_OrgSelect->setText(org_name);
    }else{
        ui->lbl_OrgSelect->setText(trUtf8("<font color = red>Организация не выбрана</font>"));
    }
    Refresh_cmBx_NumApp_onApartament();
    delete ui->tblView_Apartament->model();
    Refresh_tblView_Apartament();
}

void AdminWindow::on_pBtn_Save_clicked()
{
    int id_org;
    int id_home;
    int NumApart;

    Organization organization;
    organization.setName(ui->lbl_OrgSelect->text());
    id_org = organization.getId();

    Home home;
    home.setName(ui->lbl_HomeSelect->text());
    id_home = home.getId();

    if(ui->lEd_NumApart_onApart->text()!=""){
        NumApart = ui->lEd_NumApart_onApart->text().toInt();
    } else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Номер квартиры"),QMessageBox::Ok);
        return;
    }

    Apartment apartment(id_home,id_org,NumApart);
    if ( apartment.getId() == -1){
        apartment.New(id_home,id_org,NumApart);
        Mode("app_deff");
        Refresh_cmBx_NumApp_onApartament();
        ui->cmBx_NumAp_on_Apartament->setCurrentText(QString::number(NumApart));
        Refresh_tblView_Apartament();

    } else {
        QMessageBox::warning(this,trUtf8("Квартиры"),
                             trUtf8("Ошибка добавления новой квартиры"),QMessageBox::Ok);
        return;
    }
}

void AdminWindow::on_cmBx_NumAp_on_Apartament_activated(int index)
{
    Q_UNUSED(index);
    Refresh_tblView_Apartament();
}

void AdminWindow::on_pBtn_DeleteApartment_clicked()
{
    int id_org;
    int id_home;
    Organization organization;
    organization.setName(ui->lbl_OrgSelect->text());

    id_org = organization.getId();
    if(id_org == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    Home home;
    home.setName(ui->lbl_HomeSelect->text());
    id_home = home.getId();
    if(id_home == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    if (QMessageBox::question(this,trUtf8("Удаление"),
                              trUtf8("Удалить?"),
                              QMessageBox::Yes,QMessageBox::No) == QMessageBox::No){
        return;
    }

    Apartment apartment(id_home,id_org,ui->cmBx_NumAp_on_Apartament->currentText().toInt());
    apartment.DeleteApartment();

    Refresh_cmBx_NumApp_onApartament();
    Refresh_tblView_Apartament();
}

void AdminWindow::Refresh_tblV_on_Uslugi()
{
    int OrganiztionID = -1, HomeID = -1;
    int row;

    row = ui->cmBx_Org_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                OrganiztionID = index.data().toInt();
            }else{
                return;
            }
        }else{
            return;
        }
    }else{
        return;
    }

    row = ui->cmBx_Home_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                HomeID = index.data().toInt();
            }else{
                return;
            }
        }else{
            return;
        }
    }else{
        return;
    }

    Apartment apartment(HomeID,OrganiztionID,ui->cmBx_NumAp_on_Uslugi->currentText().toInt());


    ModelUsliga *model = new ModelUsliga;
    model->setIdApartment(apartment.getId());
    ui->tblV_on_Uslugi->setModel(model);
    ui->tblV_on_Uslugi->hideColumn(2);
    ui->tblV_on_Uslugi->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Uslugi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_on_Uslugi->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);


}

void AdminWindow::on_cmBx_Org_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void AdminWindow::on_cmBx_Home_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void AdminWindow::on_cmBx_NumAp_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void AdminWindow::on_pBtnAddUsluga_clicked()
{
    Dialog_with_LineEdit *dlg = new Dialog_with_LineEdit(this);
    connect(dlg,SIGNAL(sgn_string(QString,int)),SLOT(sl_AddUsluga(QString,int)));
    dlg->open();
}


void AdminWindow::sl_AddUsluga(const QString &usluga, const int &id_type_usluga)
{
    QStringList column, value;
    column << "name" << "type_usluga";
    value << usluga << QString::number(id_type_usluga);
    db.add("usluga",column,value);
    Refresh_tblV_on_Uslugi();
}

void AdminWindow::on_pBtnDeleteUsluga_clicked()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }else{
        db.DeleteLine("usluga","id_usluga",ui->tblV_on_Uslugi->model()->index(
                                                       ui->tblV_on_Uslugi->currentIndex().row(),2).data().toInt());
        Refresh_tblV_on_Uslugi();
    }
}

void AdminWindow::on_pBtnMassOperation_clicked()
{
    MassOperations *dlg = new MassOperations;
    int row;
    int HomeID,OrganiztionID;

    row = ui->cmBx_Home_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                HomeID = index.data().toInt();
            }else{
                return;
            }
        }else{
            return;
        }
    }else{
        return;
    }

    row = ui->cmBx_Org_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_Uslugi->model()->index(row, 0);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                OrganiztionID = index.data().toInt();
            }else{
                return;
            }
        }else{
            return;
        }
    }else{
        return;
    }

    dlg->setHome(HomeID);
    dlg->setOrganization(OrganiztionID);

    dlg->open();
}

void AdminWindow::on_pBtn_Tarif_request_clicked()
{

    if(ui->lEd_Tarif_month->text()==""||ui->lEd_Tarif_year->text()==""){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Заполните одно из полей"),QMessageBox::Ok);
        return;
    }
    if(ui->lEd_Tarif_month->text().toInt()>12
            || ui->lEd_Tarif_year->text().toInt()<1900
            || ui->lEd_Tarif_year->text().toInt()>2100){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не верная дата"),QMessageBox::Ok);
        return;
    }
        qDebug() << "test";
    ui->tblV_on_Tarif->setModel(
                tbl_tariff.ModelTarifTabl(ui->lEd_Tarif_year->text().toInt(),ui->lEd_Tarif_month->text().toInt()));
    ui->tblV_on_Tarif->hideColumn(0);
}
