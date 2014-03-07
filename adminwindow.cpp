#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "mainwindow.h"
#include <qtextcodec.h>


AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    QRegExpValidator *only_number,*dv;
    only_number = new QRegExpValidator(QRegExp("[0-9]+"),this);                 //валидатор для цифр
    dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),this);       //валидатор для чисел

    setLayout(ui->gridLayout);                                                  //главный слой

    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),SLOT(Refresh(int)));
    connect(this,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    ui->tabWidget->setCurrentIndex(0);

    // настройка вкладки Организации
    ui->tab_Organization->setLayout(ui->verticalLayout);                        //вкладка Организации
    ui->lEdAcc->setValidator(only_number);                                      // поле с лицевым счётом
    connect(ui->pBtn_addOrg,SIGNAL(clicked()),SLOT(AddOrg()));
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
    connect(ui->pBtn_addHome,SIGNAL(clicked()),SLOT(AddHome()));   // добавление дома
    connect(ui->tblView_Home,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_HomeEdit(QModelIndex)));
    connect(ui->pBtn_CancelHome,SIGNAL(clicked()),SLOT(sl_CancelHome()));
    connect(ui->pBtn_DeleteHome,SIGNAL(clicked()),SLOT(sl_DeleteHome()));
    connect(ui->pBtn_SaveHome,SIGNAL(clicked()),SLOT(sl_SaveHome()));

    //------------------------------------

    // настройка Квартиры
    ui->tab_Appartament->setLayout(ui->verticalLayout_2);                       //Вкладка квартиры
    ui->lEd_Balkon->setValidator(dv);
    ui->lEd_InhabedArea->setValidator(dv);
    ui->lEd_lodggia->setValidator(dv);
    ui->lEd_PersAcc->setValidator(only_number);
    ui->lEd_RealMen->setValidator(only_number);
    ui->lEd_RentMen->setValidator(only_number);
    ui->lEd_TotalArea->setValidator(dv);
    ui->lEd_NumApart_onApart->setValidator(only_number);

//    connect(ui->pBtn_Verify,SIGNAL(clicked()),this,SLOT(Verify()));//  проверка наличия квартиры
    connect(ui->pBtn_Save,SIGNAL(clicked()),SLOT(SaveApart()));    // сохранение изменения квартир
    connect(ui->cmBx_Home_on_App,SIGNAL(activated(int)),SLOT(Refresh_cmBx_NumApp_onApartament()));
    connect(ui->cmBx_Org_on_App,SIGNAL(activated(int)),SLOT(Refresh_cmBx_NumApp_onApartament()));
    Mode("app_deff");
    //------------------------------------


    //настройка Услуги
    ui->tab_Uslugi->setLayout(ui->verticalLayout_9);
    AdminWindow::connect(ui->pBtn_Uslug_View,SIGNAL(clicked()),SLOT(UslView()));//кнопка показать услуги
    AdminWindow::connect(ui->pBtn_AddUsluga,SIGNAL(clicked()),
                         SLOT(AddUsluga_of_Apartament()));                      //добавление услуги в квартиру
    AdminWindow::connect(ui->pBtn_DeleteUsluga,SIGNAL(clicked()),SLOT(sl_DeleteUsluga()));
    AdminWindow::connect(ui->cmBx_NumAp_on_Uslugi,SIGNAL(activated(int)),SLOT(UslView()));
    AdminWindow::connect(ui->cmBx_Home_on_Uslugi,SIGNAL(activated(int)),SLOT(Refresh_cmbNumApp_onUslugi()));
    AdminWindow::connect(ui->cmBx_Org_on_Uslugi,SIGNAL(activated(int)),SLOT(Refresh_cmbNumApp_onUslugi()));
//    Refresh_Uslugi();
    //------------------------------------

    //настройка тарифы
    ui->lEd_Tarif_year->setText(QString::number(QDate::currentDate().year()));
    ui->lEd_Tarif_month->setText(QString::number(QDate::currentDate().month()));
    ui->tab_Tarif->setLayout(ui->verticalLayout_8);

    AdminWindow::connect(ui->pBtn_Tarif_request,SIGNAL(clicked()),SLOT(TarifRequest()));
    AdminWindow::connect(ui->pBtn_TarifEdit,SIGNAL(clicked()),SLOT(TarifEdit()));
    AdminWindow::connect(ui->pBtn_Fill,SIGNAL(clicked()),SLOT(FillTarif()));
    //---------------------------------------

    //настройка пенсионеры


    ui->tab_Pensioner->setLayout(ui->mainLay_Pens);
    ui->cmBx_PensApart->setModel(
                db.ModelApartament(ui->cmBx_Home_on_Pens->model()->index(ui->cmBx_Home_on_Pens->currentIndex(),1).data().toInt()
                                   ,ui->cmBx_Org_on_Pens->model()->index(ui->cmBx_Org_on_Pens->currentIndex(),1).data().toInt()));
    ui->cmBx_PensApart->addItem("");

}

AdminWindow::~AdminWindow()
{
    delete ui;
}
void AdminWindow::AddOrg()                                                  //Добавление организации
{
    QStringList column, value;

    column<<"name"<<"bank"<<"sett_account";
    value<<ui->lEdNameOrg->text()<<ui->lEdBank->text()<<ui->lEdAcc->text();
    if (db.add("organiz",column,value) != 0){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Запись не создана"),QMessageBox::Ok);
        return;
    }
    ui->lEdAcc->setText("");
    ui->lEdBank->setText("");
    ui->lEdNameOrg->setText("");
    Refresh_Organization();
}

void AdminWindow::AddUsluga_of_Apartament()
{
    int ApartamentID,UslugaID;
    int row;
    row = ui->cmBx_NumAp_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_NumAp_on_Uslugi->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                ApartamentID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Предупреждение"),
                                     trUtf8("Не заполнены поля"),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Предупреждение"),
                                 trUtf8("Не заполнены поля"),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не заполнены поля"),QMessageBox::Ok);
        return;
    }

    row = ui->cmBx_Uslugi_onUslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Uslugi_onUslugi->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                UslugaID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле "),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле "),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле "),QMessageBox::Ok);
        return;
    }

    QStringList value,column;
    column << "id_apartament" << "id_usluga";
    value << QString::number(ApartamentID) << QString::number(UslugaID);
    if(db.add("list_app_usluga",column,value)!=0){
        return;
    }
    if(db.is_TypeUsluga(UslugaID)==1)
    {
        column.clear();
        value.clear();
        column << "id_list_app_usluga" << "date_pokazanie" << "pokazanie_home" << "pokazanie_end";
        value << QString::number(db.is_idListAppUsluga(ApartamentID,UslugaID))
              << QString::number(IsDateOfUnix(QDate::currentDate().year(),QDate::currentDate().month(),1))
              << "0" <<"0";
        db.add("pokazanie",column,value);
    }

    UslView();
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


void AdminWindow::Refresh_Appartament(int id_apartament /*= -1*/)
{
    if (ui->cmBx_Home_on_App->isEnabled() && id_apartament == -1)
        ui->cmBx_Org_on_App->setModel(db.Model("organiz"));
    if (ui->cmBx_Org_on_App->isEnabled() && id_apartament == -1)
        ui->cmBx_Home_on_App->setModel(db.Model("homes"));
    Refresh_cmBx_NumApp_onApartament();
}

void AdminWindow::SaveApart()
{
    int id_org;
    int id_home;
    int NumApart;

    id_org = isIdSelectOrganiztion_onApartament();
    if(id_org == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    id_home = isIdSelectHome_onApartament();
    if(id_home == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    if(ui->lEd_NumApart_onApart->text()!=""){
        NumApart = ui->lEd_NumApart_onApart->text().toInt();
    } else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Номер квартиры"),QMessageBox::Ok);
        return;
    }

    int t = db.is_idappart(id_home,id_org,NumApart);
    if ( t == -1){
        db.NewApatament(id_org,id_home,NumApart);
        Refresh_Appartament(db.is_idappart(id_home,id_org,NumApart));
    }

    if ((QMessageBox::question(this,trUtf8("Квартиры"),
                               trUtf8("test"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        Refresh_Appartament(db.is_idappart(id_home,id_org,NumApart));
        Mode("app_deff");
    }  else  {
        Mode("app_edit");
        Refresh_Appartament(db.is_idappart(id_home,id_org,NumApart));
    }
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
        ui->cmBx_Home_on_App->setEnabled(1);
        ui->cmBx_Org_on_App->setEnabled(1);
        ui->pBtn_NewApartament->setEnabled(1);
        ui->lEd_Balkon->setEnabled(0);
        ui->lEd_InhabedArea->setEnabled(0);
        ui->lEd_lodggia->setEnabled(0);
        ui->lEd_Name->setEnabled(0);
        ui->lEd_Partonymic->setEnabled(0);
        ui->lEd_PersAcc->setEnabled(0);
        ui->lEd_RealMen->setEnabled(0);
        ui->lEd_RentMen->setEnabled(0);
        ui->lEd_Surname->setEnabled(0);
        ui->lEd_TotalArea->setEnabled(0);
        ui->lEd_ReservMen->setEnabled(0);
        ui->lEd_NumApart_onApart->setHidden(true);
        ui->cmBx_NumAp_on_Apartament->setHidden(false);
        ui->pBtn_Cancel_onApart->setHidden(true);
        ui->pBtn_Save->setHidden(true);
        break;
    case 2: //app_edit
        ui->cmBx_Home_on_App->setEnabled(0);
        ui->cmBx_Org_on_App->setEnabled(0);
        ui->lEd_Balkon->setEnabled(1);
        ui->lEd_InhabedArea->setEnabled(1);
        ui->lEd_lodggia->setEnabled(1);
        ui->lEd_Name->setEnabled(1);
        ui->lEd_Partonymic->setEnabled(1);
        ui->lEd_PersAcc->setEnabled(1);
        ui->lEd_RealMen->setEnabled(1);
        ui->lEd_RentMen->setEnabled(1);
        ui->lEd_Surname->setEnabled(1);
        ui->lEd_TotalArea->setEnabled(1);
        ui->pBtn_Save->setEnabled(1);
        ui->lEd_ReservMen->setEnabled(1);
        break;
    case 3: //app_add
        ui->cmBx_Home_on_App->setEnabled(0);
        ui->cmBx_Org_on_App->setEnabled(0);
        ui->pBtn_NewApartament->setEnabled(0);
        ui->lEd_Balkon->setEnabled(1);
        ui->lEd_InhabedArea->setEnabled(1);
        ui->lEd_lodggia->setEnabled(1);
        ui->lEd_Name->setEnabled(1);
        ui->lEd_Partonymic->setEnabled(1);
        ui->lEd_PersAcc->setEnabled(1);
        ui->lEd_RealMen->setEnabled(1);
        ui->lEd_RentMen->setEnabled(1);
        ui->lEd_ReservMen->setEnabled(1);
        ui->lEd_Surname->setEnabled(1);
        ui->lEd_TotalArea->setEnabled(1);
        ui->lEd_NumApart_onApart->setHidden(false);
        ui->cmBx_NumAp_on_Apartament->setHidden(true);
        ui->pBtn_Cancel_onApart->setHidden(false);
        ui->pBtn_Save->setHidden(false);
        break;
    case 4:
        ui->tblV_on_Uslugi->setEnabled(0);
        ui->pBtn_AddUsluga->setEnabled(0);
        ui->cmBx_Uslugi_onUslugi->setEnabled(0);
        break;
    case 5:
        ui->tblV_on_Uslugi->setEnabled(1);
        ui->pBtn_AddUsluga->setEnabled(1);
        ui->cmBx_Uslugi_onUslugi->setEnabled(1);
        break;
    case 6: //org_default
        ui->pBtn_SaveOrg->setHidden(true);                                          //скрытие кнопки
        ui->pBtn_DeleteOrg->setHidden(true);
        ui->pBtn_CancelOrg->setHidden(true);
        ui->lEdNameOrg->setText("");                                                // очистка формы
        ui->lEdBank->setText("");
        ui->lEdAcc->setText("");
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

void AdminWindow::TarifRequest()
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

    ui->tblV_on_Tarif->setModel(
                tbl_tariff.ModelTarifTabl(ui->lEd_Tarif_year->text().toInt(),ui->lEd_Tarif_month->text().toInt()));
    ui->tblV_on_Tarif->hideColumn(0);
}

void AdminWindow::SaveTarif()
{
    int TarifID = ui->tblV_on_Tarif->model()->index(ui->tblV_on_Tarif->currentIndex().row(),0).data().toInt();

    tbl_tariff.UpdateTarif(lEd_tarif->text(),lEd_tarif2->text(),lEd_norm->text(),TarifID);
    TarifRequest();
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

void AdminWindow::UslView()
{
    int ApartamentID;
    int row;

    row = ui->cmBx_NumAp_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_NumAp_on_Uslugi->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                ApartamentID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле "),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле "),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле "),QMessageBox::Ok);
        return;
    }

    QSqlQueryModel *model;
    model = db.ModelUslugiTabl(ApartamentID);

    ui->tblV_on_Uslugi->setModel(model);
    ui->tblV_on_Uslugi->hideColumn(0);

    ui->tblV_on_Uslugi->horizontalHeader()->setStretchLastSection(false);
#ifdef HAVE_QT5
    ui->tblV_on_Uslugi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tblV_on_Uslugi->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    Mode("usl_add");
}

void AdminWindow::Refresh_Organization()
{
    delete ui->tblView_Organization->model();
    ui->tblView_Organization->setModel(db.ModelOrganiz());

    ui->tblView_Organization->horizontalHeader()->setStretchLastSection(false);
#ifdef HAVE_QT5
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
#else
    ui->tView_Organization->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tView_Organization->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->tView_Organization->horizontalHeader()->setResizeMode(1,QHeaderView::Interactive);
    ui->tView_Organization->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);
#endif

}

void AdminWindow::Refresh_Home()
{
    delete ui->tblView_Home->model();
    ui->tblView_Home->setModel(db.ModelHome());
    ui->tblView_Home->horizontalHeader()->setStretchLastSection(false);
#ifdef HAVE_QT5
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
#else
    ui->tblView_Home->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tblView_Home->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
#endif
//    ui->tblView_Home->horizontalHeader()->setResizeMode(0,QHeaderView::Interactive);
}

void AdminWindow::Refresh_Uslugi()
{

    ui->cmBx_Org_on_Uslugi->setModel(db.Model("organiz"));
    ui->cmBx_Home_on_Uslugi->setModel(db.Model("homes"));
    ui->cmBx_Uslugi_onUslugi->setModel(db.Model("usluga"));
    Refresh_cmbNumApp_onUslugi();
    Mode("usl_def");

}

void AdminWindow::Refresh_Pensioner()
{
    ui->cmBx_Org_on_Pens->setModel(db.Model("organiz"));
    ui->cmBx_Home_on_Pens->setModel(db.Model("homes"));
    ui->tblV_on_Pens->setModel(db.ModelPensioner(ui->cmBx_Home_on_Pens->model()->index(ui->cmBx_Home_on_Pens->currentIndex(),1).data().toInt()
                                                 ,ui->cmBx_Org_on_Pens->model()->index(ui->cmBx_Org_on_Pens->currentIndex(),1).data().toInt()));
    ui->tblV_on_Pens->hideColumn(0);
    ui->tblV_on_Pens->horizontalHeader()->setStretchLastSection(false);
#ifdef HAVE_QT5
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
#else
    ui->tblV_on_Pens->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tblV_on_Pens->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
#endif
}

void AdminWindow::Refresh_cmbNumApp_onUslugi()
{
    int OrganiztionID = -1, HomeID = -1;
    int row;

    row = ui->cmBx_Org_on_Uslugi->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_Uslugi->model()->index(row, 1);
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
        QModelIndex index = ui->cmBx_Home_on_Uslugi->model()->index(row, 1);
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

    QSqlQueryModel *model = new QSqlQueryModel;
    model = db.ModelApartament(HomeID,OrganiztionID);
    ui->cmBx_NumAp_on_Uslugi->setModel(model);
    Mode("usl_def");
}


void AdminWindow::Refresh_cmBx_NumApp_onApartament()
{
    int OrganiztionID = -1, HomeID = -1;
    int row;

    row = ui->cmBx_Org_on_App->currentIndex();

    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_App->model()->index(row, 1);
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

    row = ui->cmBx_Home_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_App->model()->index(row, 1);
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

    QSqlQueryModel *model = new QSqlQueryModel;
    model = db.ModelApartament(HomeID,OrganiztionID);
    ui->cmBx_NumAp_on_Apartament->setModel(model);
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
    default:
        break;
    }
}

void AdminWindow::FillTarif()
{
    tbl_tariff.FillTarif(ui->lEd_Tarif_month->text().toInt(),ui->lEd_Tarif_year->text().toInt());
    QMessageBox::information(this,trUtf8("Уведомление"),
                             trUtf8("Загрузка завершена"),QMessageBox::Ok);
    TarifRequest();
}

void AdminWindow::sl_OrgEdit(QModelIndex model)
{
    ui->lEdNameOrg->setText(ui->tblView_Organization->model()->index(model.row(),1).data().toString());
    ui->lEdBank->setText(ui->tblView_Organization->model()->index(model.row(),2).data().toString());
    ui->lEdAcc->setText(ui->tblView_Organization->model()->index(model.row(),3).data().toString());
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
    db.UpdateOrganization(ui->lEdNameOrg->text(),ui->lEdBank->text(),ui->lEdAcc->text(),
                          ui->tblView_Organization->model()->index( ui->tblView_Organization->currentIndex().row(), 0 ).data().toInt());

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

    QString err;
    err = db.DeleteOrg(ui->tblView_Organization->model()->index( ui->tblView_Organization->currentIndex().row(), 0 ).data().toInt());

    if (err != "")
    {
        QMessageBox::critical(this,trUtf8("Ошибка"),
                              err,QMessageBox::Ok);
    }
    Mode("org_default");
    Refresh_Organization();                                                     //обновление таблицы
}

void AdminWindow::sl_CancelOrg()
{
    Mode("org_default");
}

void AdminWindow::sl_DeleteUsluga()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотете удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }else{
        db.DeleteUslugaApartament(ui->tblV_on_Uslugi->model()->index(ui->tblV_on_Uslugi->currentIndex().row(),0).data().toInt());
        UslView();
    }
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

    QString err;
    err = db.DeleteHome(ui->tblView_Home->model()->index(ui->tblView_Home->currentIndex().row(),0).data().toInt());

    if (err != "")
    {
        QMessageBox::critical(this,trUtf8("Ошибка"),
                              err,QMessageBox::Ok);
    }
    Mode("home_default");
    Refresh_Home();                                                     //обновление таблицы
}

void AdminWindow::sl_SaveHome()
{
    db.UpdateHome(ui->tblView_Home->model()->index(ui->tblView_Home->currentIndex().row(),0).data().toInt(),ui->lEdHome->text());
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
    qDebug() << db.DeletePension(ui->tblV_on_Pens->model()->index(ui->tblV_on_Pens->currentIndex().row(),0).data().toInt());
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

    id_org = isIdSelectOrganiztion_onApartament();
    if(id_org == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    id_home = isIdSelectHome_onApartament();
    if(id_home == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    Mode("app_add");
    ui->lEd_NumApart_onApart->setText("");
}

int AdminWindow::isIdSelectHome_onApartament()
{
    int row;
    int id_home = -1;

    row = ui->cmBx_Home_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_App->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                id_home = index.data().toInt();
            }else{
                return -1;
            }
        }else{
            return -1;
        }
    }else{
        return -1;
    }
    return id_home;
}

int AdminWindow::isIdSelectOrganiztion_onApartament()
{
    int row;
    int id_org = -1;

    row = ui->cmBx_Org_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_App->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                id_org = index.data().toInt();
            }else{
                return -1;
            }
        }else{
            return -1;
        }
    }else{
        return -1;
    }
    return id_org;
}

void AdminWindow::on_pBtn_Cancel_onApart_clicked()
{
    Mode("app_deff");
}
