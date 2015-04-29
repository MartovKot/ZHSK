#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setLayout(ui->verticalLayout);
    ui->horizontalLayout_2->addWidget(ui->frame_obrabotka);
    ui->horizontalLayout_2->addWidget(ui->frame_admin);
    ui->frame_obrabotka->setLayout(ui->mainLayObr);
    ui->frame_admin->setLayout(ui->gridLayout);

    ui->statusBar->showMessage(BD::getDatabaseVersion());

    ////////////////////////////////////////////////////
    dlgSelCmBx_org = new Selecter_with_ComboBox(this);
    connect(dlgSelCmBx_org,SIGNAL(CurrentValue(QString)),ui->lbl_organization,SLOT(setText(QString)));
    connect(dlgSelCmBx_org,SIGNAL(CurrentValue(QString)),dlgSelCmBx_org,SLOT(close()));
    connect(dlgSelCmBx_org,SIGNAL(CurrentValue(QString)),SLOT(sl_Refresh_cmBx_NumApartanent()));
    dlgSelCmBx_org->setContentsComboBox(Organization::ModelAllOrganizationName());

    dlgSelCmBx_home = new Selecter_with_ComboBox(this);

    connect(dlgSelCmBx_home,SIGNAL(CurrentValue(QString)),ui->lbl_home,SLOT(setText(QString)));
    connect(dlgSelCmBx_home,SIGNAL(CurrentValue(QString)),dlgSelCmBx_home,SLOT(close()));
    connect(dlgSelCmBx_home,SIGNAL(CurrentValue(QString)),SLOT(sl_Refresh_cmBx_NumApartanent()));
    dlgSelCmBx_home->setContentsComboBox(Home::ModelAllHomeName());
    /////////////////////////////////////////////////////



    QFile styleFile(":/style.css");
    if( styleFile.open(QFile::ReadOnly) ) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
    }else{
        qDebug() << "Style can't be loaded.";
    }

    connect(ui->pBtnAdmin,SIGNAL(clicked()),this,SLOT(sl_Admin_mod()));
    connect(ui->pBtnOper,SIGNAL(clicked()),this,SLOT(sl_Oper_mod()));
    connect(ui->pBtnPrint,SIGNAL(clicked()),this,SLOT(sl_Print_mod()));
    connect(ui->pBtn_ArhivBlank,SIGNAL(clicked()),this,SLOT(sl_ArhivKvit()));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),SLOT(Refresh(int)));

    connect(ui->pBtn_AddPayment,SIGNAL(clicked()),SLOT(sl_addPayment()));
    connect(ui->pBtn_EditPokazanie,SIGNAL(clicked()),SLOT(sl_EditPokazanie()));
    connect(ui->pBtn_DeletePayment,SIGNAL(clicked()),SLOT(sl_DeletePayment()));
    connect(ui->tBtn_Calendar,SIGNAL(clicked()),SLOT(sl_Calendar()));
    connect(ui->tBtn_ApartLeftEnd,SIGNAL(clicked()),SLOT(sl_ApartFirst()));
    connect(ui->tBtn_ApartLeft,SIGNAL(clicked()),SLOT(sl_ApartPrevious()));
    connect(ui->tBtn_ApartRight,SIGNAL(clicked()),SLOT(sl_ApartNext()));
    connect(ui->tBtn_ApartRightEnd,SIGNAL(clicked()),SLOT(sl_ApartLast()));
    connect(ui->pBtn_NewCounter,SIGNAL(clicked()),SLOT(sl_NewCounter()));


    QRegExpValidator *only_number = new QRegExpValidator(QRegExp("[0-9]+"),this);                 //валидатор для цифр
    QRegExpValidator *dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),this); //валидатор для чисел

    ui->tabWidget->setCurrentIndex(0);

    // настройка вкладки Организации
    ui->tab_Organization->setLayout(ui->vLay_org);                        // вкладка Организации
    ui->grpBx_3->setLayout(ui->vLay_org2);
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
    ui->tab_Home->setLayout(ui->vLay_home);                              // вкладка дома
    Mode("home_default");
    connect(ui->pBtn_addHome,SIGNAL(clicked()),SLOT(AddHome()));                // добавление дома
    connect(ui->tblView_Home,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_HomeEdit(QModelIndex)));
    connect(ui->pBtn_CancelHome,SIGNAL(clicked()),SLOT(sl_CancelHome()));
    connect(ui->pBtn_DeleteHome,SIGNAL(clicked()),SLOT(sl_DeleteHome()));
    connect(ui->pBtn_SaveHome,SIGNAL(clicked()),SLOT(sl_SaveHome()));

    //------------------------------------

    // настройка Квартиры
    apartment_for_apartment = nullptr;
    ui->tab_Appartament->setLayout(ui->vLay_apartment);                       //Вкладка квартиры
    ui->lEd_NumApart_onApart->setValidator(only_number);
    Mode("app_deff");
    //------------------------------------


    //настройка Услуги
    ui->tab_Uslugi->setLayout(ui->vLay_usluga);
    //------------------------------------

    //настройка тарифы
    ui->lEd_Tarif_year->setText(QString::number(QDate::currentDate().year()));
    ui->lEd_Tarif_month->setText(QString::number(QDate::currentDate().month()));
    ui->tab_Tarif->setLayout(ui->vLay_tarif);

    connect(ui->pBtn_Tarif_request,SIGNAL(clicked()),SLOT(TarifRequest()));
    connect(ui->pBtn_TarifEdit,SIGNAL(clicked()),SLOT(TarifEdit()));
    connect(ui->pBtn_Fill,SIGNAL(clicked()),SLOT(FillTarif()));
    //---------------------------------------

    //настройка пенсионеры


    ui->tab_Pensioner->setLayout(ui->mainLay_Pens);
    Refresh_Pensioner();
    //----------------------------------------

    //настройка настройки
    ui->tab_Settings->setLayout(ui->vLay_Settings);
    Refresh_Settings();

    //Настройка обработки
    ui->lEd_Sum->setValidator(dv);
    ui->dEd_Count->setDate(QDate::currentDate());
    ui->dEd_Payment->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));

    QPixmap pixmap(":/ico/base_calendar_32.png");
    ui->tBtn_Calendar->setIcon(QIcon(pixmap));
    ui->tBtn_Calendar->setMask(pixmap.mask());

    ui->groupBox->setLayout(ui->horizontalLayout_10);
    ui->groupBox_2->setLayout(ui->horizontalLayout_5);

    ui->lbl_Payer->setHidden(true);
    //--------------------------------------------

    this->adjustSize();
    ui->frame_obrabotka->setHidden(true);
    ui->frame_admin->setHidden(true);
//    setWindowFlags(Qt::FramelessWindowHint); //Убрать внешнюю рамку

    HomeID = 1;
    OrganizationID = 1;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sl_Admin_mod()
{

//    AdmWin = new AdminWindow(this);
//    AdmWin->setPalette(this->palette());
//    connect(AdmWin,SIGNAL(finished(int)),SLOT(sl_AdminWinClose()));
//    AdmWin->show();
}

void MainWindow::sl_Oper_mod()
{
//    OperWin = new OperWindow(this);
//    OperWin->setPalette(this->palette());

//    connect(OperWin,SIGNAL(finished(int)),SLOT(sl_OperWinClose()));

//    if (isIdSelectOrganiztion() == -1){
//        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
//                             trUtf8("Не выбрана организация"),QMessageBox::Ok);
//        return;
//    }
//    if (isIdSelectHome() == -1){
//        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
//                             trUtf8("Не выбран дом"),QMessageBox::Ok);
//        return;
//    }

//    OperWin->set_parametr(isIdSelectOrganiztion(),isIdSelectHome());
//    OperWin->setWindowTitle(ui->pBtnOper->text());
//    OperWin->show();
}

void MainWindow::sl_Print_mod()
{
//    VwBlank = new ViewBlank(this);

//    QDate date;
//    date.setDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
//    DateOfUnixFormat udate(date);

//    Fast_Calculation fast_calculation(isIdSelectHome(),udate);
//    fast_calculation.fullCalc();

//    VwBlank->setDate(QDate::currentDate().year(),QDate::currentDate().month());
//    VwBlank->setInfo(isIdSelectHome(),isIdSelectOrganiztion());
//    VwBlank->generate();
//    VwBlank->show();
}

void MainWindow::sl_OpenArhiv()
{
    VwBlank = new ViewBlank;
    connect(VwBlank,SIGNAL(rejected()),SLOT(sl_test()));
    VwBlank->open_blank(CurentDirArhiv);
    VwBlank->open();
}

void MainWindow::sl_ArhivKvit()
{
    QStringList strL_dir;//список папок с квитанциями

    QDialog *dlg = new QDialog(this);
    dlg->setWindowIcon(this->windowIcon());
    dlg->setWindowTitle(trUtf8("Архив"));

    QVBoxLayout *main_layout = new QVBoxLayout(dlg);
    QPushButton *pBtn_Open = new QPushButton(trUtf8("Открыть"),dlg);
    QComboBox *cmBx_Date = new QComboBox(dlg);

    connect(cmBx_Date,SIGNAL(currentIndexChanged(QString)),SLOT(sl_setCurDirArh(QString)));
    connect(pBtn_Open,SIGNAL(clicked()),SLOT(sl_OpenArhiv()));
    connect(pBtn_Open,SIGNAL(clicked()),dlg,SLOT(close()));

    QString str_folder = "./kvit/";                      // текущтй каталог
    QDir dir;
    dir.setPath(str_folder);
    QStringList filtr;
    filtr << "*_????";
    dir.setNameFilters(filtr);
    strL_dir = dir.entryList();
    cmBx_Date->addItems(strL_dir);

    main_layout->addWidget(cmBx_Date);
    main_layout->addWidget(pBtn_Open);
    dlg->setLayout(main_layout);
    dlg->open();
}

void MainWindow::sl_setCurDirArh(QString CurDir)
{
    CurentDirArhiv = CurDir;
}

void MainWindow::setVersion(QString ver)
{
    Version = trUtf8("ЖСК-квитанция ")+ver;
    setWindowTitle(Version);
}

void MainWindow::sl_AddOrg()                                                  //Добавление организации
{
    if (Organization::New(ui->lEdNameOrg->text(), ui->lEdBank->text(), ui->lEdAcc->text(), ui->lEdINN->text())){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Организация не добавлена"),QMessageBox::Ok);
        return;
    }

    ui->lEdAcc->setText("");
    ui->lEdBank->setText("");
    ui->lEdNameOrg->setText("");
    Refresh_Organization();
}

void MainWindow::AddHome()
{
    if ( Home::createNew(ui->lEdHome->text())){
            QMessageBox::warning(this,trUtf8("Предупреждение"),
                                 trUtf8("Запись не создана"),QMessageBox::Ok);
            return;
        }
    ui->lEdHome->setText("");
    Refresh_Home();
}


void MainWindow::Refresh_Appartament()
{
    ui->cmBx_Home_on_Apartment->setModel(Home::ModelAllHomeName());
    ui->cmBx_Org_on_Apartment->setModel(Organization::ModelAllOrganizationName());

    Refresh_cmBx_NumApp_onApartament();
    Refresh_tblView_Apartament();
}

void MainWindow::Refresh_tblView_Apartament()
{
    MyItemDelegate * dDeleg = new MyItemDelegate ();
    Home home(ui->cmBx_Home_on_Apartment->currentText());
    Organization organization(ui->cmBx_Org_on_Apartment->currentText());
    Apartment *apartment = new Apartment(home.getId(),
                        organization.getId(),
                        ui->cmBx_NumAp_on_Apartament->currentText().toInt(),this);

    TransposeProxyModel *trans = new TransposeProxyModel(this);
    trans->setSourceModel(apartment->ModelOneApartment());
    ui->tblView_Apartament->setModel(trans);
    connect(apartment,SIGNAL(sgn_EditModel()),SLOT(Refresh_tblView_Apartament()));

    ui->tblView_Apartament->setItemDelegate(dDeleg);

    ui->tblV_on_Uslugi->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::Mode(QString m)
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
        ui->pBtn_NewApartament->setEnabled(1);

        ui->lEd_NumApart_onApart->setHidden(true);
        ui->cmBx_NumAp_on_Apartament->setHidden(false);
        ui->pBtn_Cancel_onApart->setHidden(true);
        ui->pBtn_Save->setHidden(true);
        ui->tblView_Apartament->setHidden(false);

        break;
    case 2: //app_edit
        ui->pBtn_Save->setEnabled(1);
        break;
    case 3: //app_add
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

void MainWindow::TarifRequest()
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

void MainWindow::SaveTarif()
{
    int TarifID = ui->tblV_on_Tarif->model()->index(ui->tblV_on_Tarif->currentIndex().row(),0).data().toInt();

    tbl_tariff.UpdateTarif(lEd_tarif->text(),lEd_tarif2->text(),lEd_norm->text(),TarifID);
    TarifRequest();
    dlg->close();
    delete dlg;
}

void MainWindow::TarifEdit()
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
    connect(pBtn_TarifSave,SIGNAL(clicked()),SLOT(SaveTarif()));

    main_lay->addLayout(lay_1);
    main_lay->addLayout(lay_2);

    main_lay->addLayout(lay_3);
    dlg->setLayout(main_lay);
    dlg->setModal(true);
    dlg->show();
}

void MainWindow::Refresh_Organization()
{
    delete ui->tblView_Organization->model();
    ui->tblView_Organization->setModel(Organization::ModelAllOrganizationFull());
    ui->tblView_Organization->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tblView_Organization->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
}

void MainWindow::Refresh_Home()
{
    delete ui->tblView_Home->model();
    ui->tblView_Home->setModel(Home::ModelAllHomeFull());
    ui->tblView_Home->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblView_Home->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
}

void MainWindow::Refresh_Uslugi()
{
    ui->cmBx_Home_on_Uslugi->setModel(Home::ModelAllHomeName());
    ui->cmBx_Org_on_Uslugi->setModel(Organization::ModelAllOrganizationName());

    Refresh_cmbNumApp_onUslugi();
    Refresh_tblV_on_Uslugi();

    Mode("usl_def");

}

void MainWindow::Refresh_Pensioner()
{
    ui->cmBx_Home_on_Pens->setModel(Home::ModelAllHomeName());
    ui->cmBx_Org_on_Pens->setModel(Organization::ModelAllOrganizationName());
    Home home(ui->cmBx_Home_on_Pens->currentText());
    ui->cmBx_PensApart->setModel(home.ModelAllApartamentNumberWithFIO());
    ui->cmBx_PensApart->addItem("");
    ui->tblV_on_Pens->setModel(Apartment::ModelPensionerLivingAlone(home.getId(),home.organization()->getId()));

    ui->tblV_on_Pens->hideColumn(0);
    ui->tblV_on_Pens->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_on_Pens->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
}

void MainWindow::Refresh_cmbNumApp_onUslugi()
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
    model = Apartment::ModelAllApartment(HomeID,OrganiztionID);
    ui->cmBx_NumAp_on_Uslugi->setModel(model);

}


void MainWindow::Refresh_cmBx_NumApp_onApartament()
{
    Home home(ui->cmBx_Home_on_Apartment->currentText());
    ui->cmBx_NumAp_on_Apartament->setModel(home.ModelAllApartamentNumber());
    Mode("app_def");
}

void MainWindow::sl_Refresh_cmBx_NumApartanent()
{
    Organization organization(ui->lbl_organization->text());
    Home home(ui->lbl_home->text());

    if(home.getId() != -1 && organization.getId() != -1){
        ui->cmBx_NumApartanent->setModel(Apartment::ModelAllApartment(home.getId(),organization.getId()));
    }
}

void MainWindow::Refresh(int num_tab)
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

void MainWindow::FillTarif()
{
    tbl_tariff.FillTarif(ui->lEd_Tarif_month->text().toInt(),ui->lEd_Tarif_year->text().toInt());
    QMessageBox::information(this,trUtf8("Уведомление"),
                             trUtf8("Загрузка завершена"),QMessageBox::Ok);
    TarifRequest();
}

void MainWindow::sl_OrgEdit(QModelIndex model)
{
    ui->lEdNameOrg->setText(ui->tblView_Organization->model()->index(model.row(),1).data().toString());
    ui->lEdBank->setText(ui->tblView_Organization->model()->index(model.row(),2).data().toString());
    ui->lEdAcc->setText(ui->tblView_Organization->model()->index(model.row(),3).data().toString());
    ui->lEdINN->setText(ui->tblView_Organization->model()->index(model.row(),4).data().toString());
    Mode("org_edit");

}

void MainWindow::sl_HomeEdit(QModelIndex model)
{
    ui->lEdHome->setText(ui->tblView_Home->model()->index(model.row(),1).data().toString());
    Mode("home_edit");
}

void MainWindow::sl_SaveOrg()
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
void MainWindow::sl_DeleteOrg()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    Organization organization(ui->lEdNameOrg->text());
    organization.deleteFromDB();
    Mode("org_default");
    Refresh_Organization();                                                     //обновление таблицы
}

void MainWindow::sl_CancelOrg()
{
    Mode("org_default");
}

void MainWindow::sl_CancelHome()
{
    Mode("home_default");
}

void MainWindow::sl_DeleteHome()
{
    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    Home home(ui->lEdHome->text());
    home.deleteFromDB();
    Mode("home_default");
    Refresh_Home();                                                     //обновление таблицы
}

void MainWindow::sl_SaveHome()
{
    Home home(ui->tblView_Home->model()->index(
                       ui->tblView_Home->currentIndex().row(),0).data().toInt());
    home.rename(ui->lEdHome->text());
    Refresh_Home();
    Mode("home_default");
}

void MainWindow::on_pBtn_addPens_clicked()
{
    QString id_apart;
    id_apart = ui->cmBx_PensApart->model()->index(ui->cmBx_PensApart->currentIndex(),1).data().toString();
    db.add("pensioner_living_alone","id_apartament",id_apart);
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(false);
    Refresh_Pensioner();
}

void MainWindow::on_pBtn_delPens_clicked()
{
    Home home(ui->cmBx_Home_on_Pens->currentText());
    Apartment apartment(home.getId(),
                        home.organization()->getId(),
                        ui->tblV_on_Pens->model()->index(ui->tblV_on_Pens->currentIndex().row(),0).data().toInt()
                        );
    apartment.setIslivingAlonePensioner(false);
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(false);
    Refresh_Pensioner();
}

void MainWindow::on_cmBx_PensApart_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->pBtn_addPens->setEnabled(true);
    ui->pBtn_delPens->setEnabled(false);
}

void MainWindow::on_tblV_on_Pens_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    ui->pBtn_addPens->setEnabled(false);
    ui->pBtn_delPens->setEnabled(true);
}

void MainWindow::on_pBtn_NewApartament_clicked()
{
    Organization organization(ui->cmBx_Org_on_Apartment->currentText());

    if(organization.getId() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    Home home(ui->cmBx_Home_on_Apartment->currentText());
    if(home.getId() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    Mode("app_add");

    ui->lEd_NumApart_onApart->setText("");
}

void MainWindow::on_pBtn_Cancel_onApart_clicked()
{
    Mode("app_deff");
}

void MainWindow::Refresh_Settings()
{
    ui->tblV_on_Settings->setModel(Settings::Model());

    ui->tblV_on_Settings->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Settings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_tBtn_AddSetting_clicked()
{
    QStringList column, value;
    column << "name_setting" << "value_setting";
    value << "" << "";
    db.add("settings", column, value);
    Refresh_Settings();
}

void MainWindow::on_tBtn_DeleteSetting_clicked()
{
    Settings::Delete(ui->tblV_on_Settings->model()->index(ui->tblV_on_Settings->currentIndex().row(),0).data().toString());
    Refresh_Settings();
}

void MainWindow::on_tBtn_EditSetting_clicked()
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


void MainWindow::on_pBtn_Save_clicked()
{
    int NumApart;

    Organization organization(ui->cmBx_Org_on_Apartment->currentText());
    Home home(ui->cmBx_Home_on_Apartment->currentText());


    if(ui->lEd_NumApart_onApart->text()!=""){
        NumApart = ui->lEd_NumApart_onApart->text().toInt();
    } else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Номер квартиры"),QMessageBox::Ok);
        return;
    }

    if (Apartment::New(home.getId(),organization.getId(),NumApart)){
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

void MainWindow::on_cmBx_NumAp_on_Apartament_activated(int index)
{
    Q_UNUSED(index);
    Refresh_tblView_Apartament();
}

void MainWindow::on_pBtn_DeleteApartment_clicked()
{
    Organization organization(ui->cmBx_Org_on_Apartment->currentText());

    if(organization.getId() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Организация "),QMessageBox::Ok);
        return;
    }

    Home home(ui->cmBx_Home_on_Apartment->currentText());
    if(home.getId() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не заполнено поле Дом "),QMessageBox::Ok);
        return;
    }

    if (QMessageBox::question(this,trUtf8("Удаление"),
                              trUtf8("Удалить?"),
                              QMessageBox::Yes,QMessageBox::No) == QMessageBox::No){
        return;
    }

    Apartment apartment(home.getId(), organization.getId(), ui->cmBx_NumAp_on_Apartament->currentText().toInt());
    apartment.DeleteApartment();

    Refresh_cmBx_NumApp_onApartament();
    Refresh_tblView_Apartament();
}

void MainWindow::Refresh_tblV_on_Uslugi()
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

void MainWindow::on_cmBx_Org_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void MainWindow::on_cmBx_Home_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void MainWindow::on_cmBx_NumAp_on_Uslugi_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    Refresh_tblV_on_Uslugi();
}

void MainWindow::on_pBtnAddUsluga_clicked()
{
    Dialog_with_LineEdit *dlg = new Dialog_with_LineEdit(this);
    connect(dlg,SIGNAL(sgn_string(QString,int)),SLOT(sl_AddUsluga(QString,int)));
    dlg->open();
}


void MainWindow::sl_AddUsluga(const QString &usluga, const int &id_type_usluga)
{
    QStringList column, value;
    column << "name" << "type_usluga";
    value << usluga << QString::number(id_type_usluga);
    db.add("usluga",column,value);
    Refresh_tblV_on_Uslugi();
}

void MainWindow::on_pBtnDeleteUsluga_clicked()
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

void MainWindow::Refresh_tblVPayment(int ApartamenID)
{
    Table_Payment t_payment;
    ui->tblV_Payment->setModel(t_payment.ModelPayment(ApartamenID));
    ui->tblV_Payment->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Payment->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_Payment->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Interactive);
}

void MainWindow::sl_addPayment()
{
    QString payment;
    int day, month,year;

    Home home(ui->lbl_home->text());
    Organization organization(ui->lbl_organization->text());
    Apartment apartment(home.getId(), organization.getId(), ui->cmBx_NumApartanent->currentText().toInt());

    payment = ui->lEd_Sum->text();
    if (payment == ""){
        return;
    }
    day = ui->dEd_Payment->date().day();
    month = ui->dEd_Payment->date().month();
    year = ui->dEd_Payment->date().year();
    QDate date_calc;
    date_calc.setDate(year,month,day);
    if (date_calc.daysTo(QDate::currentDate())>40 ){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Старая оплата. \nПроверте дату."),QMessageBox::Ok);
        return;
    }
    if (date_calc>QDate::currentDate()){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Уже изобрели машину времени? \nПроверте дату ;)"),QMessageBox::Ok);
        return;
    }
    Table_Payment t_payment;
    t_payment.add_line(QString::number(apartment.getId()),
                       QString::number(year),
                       QString::number(month),
                       QString::number(day),payment);

    Refresh_tblVPayment(apartment.getId());
    Refresh_LabelPayAndDebt(/*apartment.getId()*/);

}

void MainWindow::Refresh_tblVCount()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    ui->tblV_Count->setModel(Indications::ModelPokazanie(apartment.getId(),
                                 ui->dEd_Count->date().month(),
                                 ui->dEd_Count->date().year()));
    Indications::New(apartment.getId(),                     //новое показание на след месяц
                     ui->dEd_Count->date().month(),
                     ui->dEd_Count->date().year());
    ui->tblV_Count->hideColumn(0);
    ui->tblV_Count->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Count->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_Count->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);

}

void MainWindow::sl_EditPokazanie()
{   
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());
    EditIndicationsCounter *dlg = new EditIndicationsCounter(apartment.getId(),
                                                            ui->dEd_Count->date().month(),
                                                            ui->dEd_Count->date().year(),
                                                            this);
    dlg->setModal(true);
    dlg->setPalette(this->palette());
    connect(dlg,SIGNAL(finished(int)),dlg,SLOT(deleteLater()));
    connect(dlg,SIGNAL(finished(int)),SLOT(Refresh_tblVCount()));
    connect(dlg,SIGNAL(finished(int)),SLOT(sl_RefreshLabel()));


    dlg->show();

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()){
    case Qt::Key_Enter :
    case Qt::Key_Return:
        if(ui->cmBx_NumApartanent == focusWidget()){
            ui->dEd_Payment->setFocus();
        }else if(focusWidget() == ui->dEd_Count){
            ui->cmBx_NumApartanent->setFocus();
        }else if(focusWidget() == ui->dEd_Payment){
            ui->lEd_Sum->setFocus();
        }else if(focusWidget() == ui->lEd_Sum){
            ui->pBtn_AddPayment->click();
            ui->pBtn_EditPokazanie->setFocus();
        }
        break;
    default: break;
    }
}

void MainWindow::sl_DeletePayment()
{
    int row;
    int month,year,day;

    row = ui->tblV_Payment->currentIndex().row();
    if (row<0){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не выбрана оплата"),QMessageBox::Ok);
        return;
    }

    if (ui->tblV_Payment->model()->index(row,0).data().toString().count() != 10){
        qDebug() << "wrong format";
        return;
    }

    day = ui->tblV_Payment->model()->index(row,0).data().toString().left(2).toInt();
    month = ui->tblV_Payment->model()->index(row,0).data().toString().mid(3,2).toInt();
    year = ui->tblV_Payment->model()->index(row,0).data().toString().right(4).toInt();

    if(QMessageBox::question(this,trUtf8("Удаление"),
                         trUtf8("Удалить эту запись?\n")+QString::number(day)+" "+
                         QDate::longMonthName(month)+" "+QString::number(year)+trUtf8(" года"),
                    QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok){
        QString err;
        Table_Payment t_payment;

        Home home(ui->lbl_home->text());
        Organization organization(ui->lbl_organization->text());
        Apartment apartment(home.getId(), organization.getId(), ui->cmBx_NumApartanent->currentText().toInt());

        err = t_payment.delete_Payment(apartment.getId(), year,month,day);
        if (err == ""){
            QMessageBox::information(this,trUtf8("Успех"),
                                 trUtf8("Удаление прошло успешно \n")+err,QMessageBox::Ok);
            Refresh_tblVPayment(apartment.getId());
            Refresh_LabelPayAndDebt(/*apartment.getId()*/);
        }else{
            QMessageBox::critical(this,trUtf8("Ошибка"),
                                 trUtf8("Удаление не произошло \n")+err,QMessageBox::Ok);
        }
    }
}

void MainWindow::sl_Calendar()
{
    QDialog *w = new QDialog(this);
    QCalendarWidget *calendar = new QCalendarWidget(w);
    QHBoxLayout *mainlayout = new QHBoxLayout;
    mainlayout->addWidget(calendar);
    w->setLayout(mainlayout);
    connect(calendar,SIGNAL(clicked(QDate)),ui->dEd_Payment,SLOT(setDate(QDate)));
    connect(calendar,SIGNAL(clicked(QDate)),w,SLOT(close()));
    ui->lEd_Sum->setFocus();
    w->open();
}

void MainWindow::Refresh_LabelPayAndDebt() //обновление выводяшейся оплаты и долга
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    int month = ui->dEd_Count->date().month();
    int year = ui->dEd_Count->date().year();
    DateOfUnixFormat date_calc(year,month,1);

    if (ui->dEd_Count->date() == QDate::currentDate()){ //Расчёт производим только за текущий месяц
        Fast_Calculation calculate(HomeID,date_calc);
        calculate.fullCalc();
    }
    ui->lblInPayment->setText(QString::number(Fast_Calculation::AmountToPay(apartment.getId(),date_calc.Second())));
    ui->lblDolg->setText(Fast_Calculation::Debt(apartment.getId(),date_calc));
}

void MainWindow::sl_ApartFirst()
{
    ui->cmBx_NumApartanent->setCurrentIndex(0);
    sl_RefreshFull();
}

void MainWindow::sl_ApartLast()
{
    ui->cmBx_NumApartanent->setCurrentIndex(ui->cmBx_NumApartanent->count()-1);
    sl_RefreshFull();
}

void MainWindow::sl_ApartNext()
{
    int index;
    index = ui->cmBx_NumApartanent->currentIndex();
    if(index < ui->cmBx_NumApartanent->count()-1){
        index++;
    }
    ui->cmBx_NumApartanent->setCurrentIndex(index);
    sl_RefreshFull();
}

void MainWindow::sl_ApartPrevious()
{
    int index;

    index = ui->cmBx_NumApartanent->currentIndex();
    if(index > 0){
        index--;
    }
    ui->cmBx_NumApartanent->setCurrentIndex(index);
    sl_RefreshFull();
}

void MainWindow::sl_NewCounter()//вызывается когда происходит смена счётчика
{
    int id_counter,row;

    row = ui->tblV_Count->currentIndex().row(); //Номер строки
    if(row == -1){
        QMessageBox::warning(this,trUtf8("Предупреждение!"),
                             trUtf8("Не выбран счётчик. \nВыберите нужный и повторите ещё раз."),QMessageBox::Ok);
        return;
    }
    id_counter = ui->tblV_Count->model()->index(row,0).data().toInt(); //ID Показания Счётчика

    NewCounter *dlg = new NewCounter(this);
    dlg->set_IdPokazanie(id_counter);
    dlg->open();
}

void MainWindow::on_pBtn_NewCounterNext_clicked()
{
    int id_counter = idCurrentCounter();
    if(id_counter == -1 ){
        return;
    }

    NewCounter *dlg = new NewCounter(this);
    dlg->set_IdPokazanie(Indications::New(id_counter,"0"));
    dlg->open();
}

void MainWindow::Refresh_lbl_Payer()
{   
    Home home(ui->lbl_home->text());
    Organization organization(ui->lbl_organization->text());
    Apartment apartment(home.getId(), organization.getId(), ui->cmBx_NumApartanent->currentText().toInt());

    ui->lbl_Payer->setText(apartment.is_FIO_payer());
}


void MainWindow::on_tBtn_org_clicked()
{
    dlgSelCmBx_org->show();
}

void MainWindow::on_tBtn_Home_clicked()
{
    dlgSelCmBx_home->show();
}


void MainWindow::on_cmBx_NumApartanent_currentIndexChanged(const QString &arg1)
{
    Home home(ui->lbl_home->text());
    Organization organization(ui->lbl_organization->text());
    Apartment apartment(home.getId(), organization.getId(), arg1.toInt());


//    Refresh_tblVPayment(apartment.getId());
//    Refresh_tblVCount();
//    Refresh_LabelPayAndDebt(apartment.getId());
}

void MainWindow::sl_Refresh_tblVCount()
{
    Refresh_tblVCount();
}

void MainWindow::sl_RefreshFull()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    Refresh_tblVPayment(apartment.getId());
    Refresh_tblVCount();
    Refresh_LabelPayAndDebt();
    Refresh_lbl_Payer();
}

int MainWindow::idCurrentCounter()
{
    int row;

    row = ui->tblV_Count->currentIndex().row(); //Номер строки
    if(row == -1){
        QMessageBox::warning(this,trUtf8("Предупреждение!"),
                             trUtf8("Не выбран счётчик. \nВыберите нужный и повторите ещё раз."),QMessageBox::Ok);
        return -1;
    }
    return ui->tblV_Count->model()->index(row,0).data().toInt(); //ID Показания Счётчика
}
