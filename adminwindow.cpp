#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "mainwindow.h"
#include <qtextcodec.h>
///dferhg;etrho;

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);



    QRegExpValidator *only_number,*dv;
    only_number = new QRegExpValidator(QRegExp("[0-9]+"),this);                 //валидатор для цифр
    dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),this);       //валидатор для чисел

    setLayout(ui->gridLayout);                                                  //главный слой

    AdminWindow::connect(ui->tabWidget,SIGNAL(currentChanged(int)),SLOT(Refresh()));

    // настройка вкладки Организации
    ui->tab_Organization->setLayout(ui->verticalLayout);                        //вкладка Организации
    ui->lEdAcc->setValidator(only_number);                                      // поле с лицевым счётом
    AdminWindow::connect(ui->pBtn_addOrg,SIGNAL(clicked()),SLOT(AddOrg()));
    connect(ui->tView_Organization,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_OrgEdit(QModelIndex)));
    connect(ui->pBtn_SaveOrg,SIGNAL(clicked()),SLOT(sl_SaveOrg()));
    connect(ui->pBtn_DeleteOrg,SIGNAL(clicked()),SLOT(sl_DeleteOrg()));
    connect(ui->pBtn_CancelOrg,SIGNAL(clicked()),SLOT(sl_CancelOrg()));
    ui->pBtn_SaveOrg->setHidden(true);
    ui->pBtn_DeleteOrg->setHidden(true);
    ui->pBtn_CancelOrg->setHidden(true);
    ID_ORG = -1;

    Refresh_Organization();
    // -------------------------------


    // настройка вкладки Дома
    ui->tab_Home->setLayout(ui->verticalLayout_3);                              // вкладка дома
    AdminWindow::connect(ui->pBtn_addHome,SIGNAL(clicked()),SLOT(AddHome()));   // добавление дома
    ID_HOME = -1;
    Mode("home_default");
    connect(ui->tblView_Home,SIGNAL(doubleClicked(QModelIndex)),SLOT(sl_HomeEdit(QModelIndex)));
    connect(ui->pBtn_CancelHome,SIGNAL(clicked()),SLOT(sl_CancelHome()));
    connect(ui->pBtn_DeleteHome,SIGNAL(clicked()),SLOT(sl_DeleteHome()));
    connect(ui->pBtn_SaveHome,SIGNAL(clicked()),SLOT(sl_SaveHome()));

    Refresh_Home();                                                             // обновление вкладки
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
    ui->lEd_NumApp->setValidator(only_number);

    AdminWindow::connect(ui->pBtn_Verify,SIGNAL(clicked()),this,SLOT(Verify()));//  проверка наличия квартиры
    AdminWindow::connect(ui->pBtn_Save,SIGNAL(clicked()),SLOT(SaveApart()));    // сохранение изменения квартир

    Refresh_Appartament();
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
    Refresh_Uslugi();
    //------------------------------------


    //настройка тарифы
    ui->lEd_Tarif_year->setText(QString::number(QDate::currentDate().year()));
    ui->lEd_Tarif_month->setText(QString::number(QDate::currentDate().month()));
    ui->tab_Tarif->setLayout(ui->verticalLayout_8);

    AdminWindow::connect(ui->pBtn_Tarif_request,SIGNAL(clicked()),SLOT(TarifRequest()));
    AdminWindow::connect(ui->pBtn_TarifEdit,SIGNAL(clicked()),SLOT(TarifEdit()));
    AdminWindow::connect(ui->pBtn_Fill,SIGNAL(clicked()),SLOT(FillTarif()));
    //---------------------------------------

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
    db.add("organiz",column,value);
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
        column << "id_list_app_usluga" << "date_month"
               << "date_year" << "pokazanie_home" << "pokazanie_end";
        value << QString::number(db.is_idListAppUsluga(ApartamentID,UslugaID))
              << QString::number(QDate::currentDate().month())
              << QString::number(QDate::currentDate().year()) << "0" <<"0";
        db.add("pokazanie",column,value);
    }

    UslView();
}

void AdminWindow::AddHome()
{
    QStringList column, value;
    column<<"name";
    value<<ui->lEdHome->text();
    db.add("homes",column,value);
    ui->lEdHome->setText("");
    Refresh_Home();
}


void AdminWindow::Refresh_Appartament(int id_apartament)
{
    if (ui->cmBx_Home_on_App->isEnabled() && id_apartament == -1)
        ui->cmBx_Org_on_App->setModel(db.Model("organiz"));
    if (ui->cmBx_Org_on_App->isEnabled() && id_apartament == -1)
        ui->cmBx_Home_on_App->setModel(db.Model("homes"));

    if(ui->lEd_NumApp->text()!="" && !ui->lEd_NumApp->isEnabled() && id_apartament != -1){
        QSqlQueryModel *model;
        model = db.ModelApartament(id_apartament);
        ui->lEd_PersAcc->setText(
                    model->index(0,model->query().record().indexOf("personal_account")).data().toString());
        ui->lEd_TotalArea->setText(
                    model->index(0,model->query().record().indexOf("total_area")).data().toString());
        ui->lEd_InhabedArea->setText(
                    model->index(0,model->query().record().indexOf("inhabed_area")).data().toString());
        ui->lEd_Balkon->setText(
                    model->index(0,model->query().record().indexOf("balkon")).data().toString());
        ui->lEd_lodggia->setText(
                    model->index(0,model->query().record().indexOf("loggia")).data().toString());
        ui->lEd_RealMen->setText(
                    QString::number(db.is_RealMen(id_apartament,QDate::currentDate().year(),QDate::currentDate().month())));
        ui->lEd_RentMen->setText(
                    QString::number(db.is_RentMen(id_apartament,QDate::currentDate().year(),QDate::currentDate().month())));
        ui->lEd_ReservMen->setText(
                    QString::number(db.is_ReservMen(id_apartament,QDate::currentDate().year(),QDate::currentDate().month())));
        ui->lEd_Surname->setText(
                    model->index(0,model->query().record().indexOf("surname")).data().toString());
        ui->lEd_Name->setText(
                    model->index(0,model->query().record().indexOf("name")).data().toString());
        ui->lEd_Partonymic->setText(
                    model->index(0,model->query().record().indexOf("patronymic")).data().toString());
    }
}
bool AdminWindow::Verify()//  Кнопка проверка
{
    int OrganiztionID = -1, HomeID = -1, NumApart = -1;
    int row;

    row = ui->cmBx_Org_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_App->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                OrganiztionID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
                return false;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
            return false;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
        return false;
    }

    row = ui->cmBx_Home_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_App->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                HomeID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
                return false;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
            return false;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле дом"),QMessageBox::Ok);
        return false;
    }

    if(ui->lEd_NumApp->text()!=""){
        NumApart = ui->lEd_NumApp->text().toInt();
    } else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Нет данных в поле номер квартиры"),QMessageBox::Ok);
        return false;
    }

    if (db.is_idappart(HomeID,OrganiztionID,NumApart) == -1){
        if ((QMessageBox::question(this,trUtf8("Квартиры нет"),
                                   trUtf8("Добавить информацию о квартире? "),
                                   QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
            return false;
        }  else  {
            db.NewApatament(OrganiztionID,HomeID,NumApart);
            Mode("app_edit");
            Refresh_Appartament(db.is_idappart(HomeID,OrganiztionID,NumApart));
        }
    }else{
        if ((QMessageBox::question(this,trUtf8("Квартира существует"),
                                   trUtf8("Начать редактирование?"),
                                   QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
            Refresh_Appartament(db.is_idappart(HomeID,OrganiztionID,NumApart));
            Mode("app_deff");
            return false;
        }  else  {
            Mode("app_edit");
            Refresh_Appartament(db.is_idappart(HomeID,OrganiztionID,NumApart));
        }
    }
    return true;
}

void AdminWindow::SaveApart()
{
    int OrganiztionID = -1, HomeID = -1, NumApart = -1;
    int row;

    row = ui->cmBx_Org_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org_on_App->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                OrganiztionID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле организация"),QMessageBox::Ok);
        return;
    }

    row = ui->cmBx_Home_on_App->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home_on_App->model()->index(row, 1);
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

    if(ui->lEd_NumApp->text()!=""){
        NumApart = ui->lEd_NumApp->text().toInt();
    } else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Нет данных в поле номер квартиры"),QMessageBox::Ok);
        return;
    }
    //-------------------------------------
    QStringList col, val;
    Mode("app_deff");
    col << "personal_account" << "total_area" << "inhabed_area"
        << "balkon "          << "loggia"     << "surname"
        << "name"             << "patronymic";

    val << ui->lEd_PersAcc->text() << ui->lEd_TotalArea->text() << ui->lEd_InhabedArea->text()
        << ui->lEd_Balkon->text()  << ui->lEd_lodggia->text()   << ui->lEd_Surname->text()
        << ui->lEd_Name->text()    << ui->lEd_Partonymic->text();

    db.UpdateApartament(col,val,db.is_idappart(HomeID,OrganiztionID,NumApart));

    col.clear();
    col << "real_men" << "rent_men" << "reserv_men";

    val.clear();
    val << ui ->lEd_RealMen->text() << ui->lEd_RentMen->text() << ui->lEd_ReservMen->text();

    db.UpdateMenInApartament(col,val,db.is_idappart(HomeID,OrganiztionID,NumApart),
                             QDate::currentDate().year(),QDate::currentDate().month());
}

void AdminWindow::Mode(QString m)
{
    int i;
    if (m=="app_deff")
        i=1;
    if (m=="app_edit")
        i=2;
    if (m=="btn_add")
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
    case  1:
        ui->cmBx_Home_on_App->setEnabled(1);
        ui->cmBx_Org_on_App->setEnabled(1);
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
        ui->pBtn_Save->setEnabled(0);
        ui->lEd_ReservMen->setEnabled(0);
        ui->lEd_NumApp->setEnabled(1);
        ui->pBtn_Verify->setEnabled(1);
        break;
    case 2:
        ui->cmBx_Home_on_App->setEnabled(0);
        ui->cmBx_Org_on_App->setEnabled(0);
        ui->lEd_NumApp->setEnabled(0);
        ui->pBtn_Verify->setEnabled(0);
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
    case 3:
        ui->cmBx_Home_on_App->setEnabled(0);
        ui->cmBx_Org_on_App->setEnabled(0);
        ui->lEd_NumApp->setEnabled(0);
        ui->pBtn_Verify->setEnabled(0);
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
        ui->pBtn_Save->setEnabled(0);
        ui->lEd_ReservMen->setEnabled(0);
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
    case 6:
        ui->pBtn_SaveOrg->setHidden(true);                                          //скрытие кнопки
        ui->pBtn_DeleteOrg->setHidden(true);
        ui->pBtn_CancelOrg->setHidden(true);
        ui->lEdNameOrg->setText("");                                                // очистка формы
        ui->lEdBank->setText("");
        ui->lEdAcc->setText("");
        break;
    case 7:
        ui->pBtn_SaveOrg->setHidden(false);                                          //скрытие кнопки
        ui->pBtn_DeleteOrg->setHidden(false);
        ui->pBtn_CancelOrg->setHidden(false);
        break;
    case 8:
        ui->pBtn_SaveHome->setHidden(true);
        ui->pBtn_DeleteHome->setHidden(true);
        ui->pBtn_CancelHome->setHidden(true);
        ui->lEdHome->setText("");
        break;
    case 9:
        ui->pBtn_SaveHome->setHidden(false);
        ui->pBtn_DeleteHome->setHidden(false);
        ui->pBtn_CancelHome->setHidden(false);
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
                db.ModelTarifTabl(ui->lEd_Tarif_year->text().toInt(),ui->lEd_Tarif_month->text().toInt()));
    ui->tblV_on_Tarif->hideColumn(0);
}

void AdminWindow::SaveTarif()
{
    int TarifID = ui->tblV_on_Tarif->model()->index(ui->tblV_on_Tarif->currentIndex().row(),0).data().toInt();

    db.UpdateTarif(lEd_tarif->text(),lEd_tarif2->text(),lEd_norm->text(),TarifID);
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

    ui->tblV_on_Uslugi->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_on_Uslugi->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    Mode("usl_add");
}

void AdminWindow::Refresh_Organization()
{
    delete ui->tView_Organization->model();
    ui->tView_Organization->setModel(db.ModelOrganiz());

    ui->tView_Organization->horizontalHeader()->setStretchLastSection(false);
    ui->tView_Organization->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tView_Organization->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->tView_Organization->horizontalHeader()->setResizeMode(1,QHeaderView::Interactive);
    ui->tView_Organization->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);

}

void AdminWindow::Refresh_Home()
{
    delete ui->tblView_Home->model();
    ui->tblView_Home->setModel(db.ModelHome());
    ui->tblView_Home->horizontalHeader()->setStretchLastSection(false);
    ui->tblView_Home->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tblView_Home->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
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

void AdminWindow::Refresh()
{
    Refresh_Appartament();
    Refresh_Home();
    Refresh_Organization();
    Refresh_Uslugi();
}

void AdminWindow::FillTarif()
{

    db.FillTarif(ui->lEd_Tarif_month->text().toInt(),ui->lEd_Tarif_year->text().toInt());
    QMessageBox::information(this,trUtf8("Уведомление"),
                             trUtf8("Загрузка завершена"),QMessageBox::Ok);
    TarifRequest();
}

void AdminWindow::sl_OrgEdit(QModelIndex model)
{


    ui->lEdNameOrg->setText(ui->tView_Organization->model()->index(model.row(),1).data().toString());
    ui->lEdBank->setText(ui->tView_Organization->model()->index(model.row(),2).data().toString());
    ui->lEdAcc->setText(ui->tView_Organization->model()->index(model.row(),3).data().toString());
    ID_ORG = ui->tView_Organization->model()->index(model.row(),0).data().toInt();
    Mode("org_edit");

}
void AdminWindow::sl_HomeEdit(QModelIndex model)
{


    ui->lEdHome->setText(ui->tblView_Home->model()->index(model.row(),1).data().toString());
    ID_HOME = ui->tblView_Home->model()->index(model.row(),0).data().toInt();
    Mode("home_edit");


}
void AdminWindow::sl_SaveOrg()
{


    if (ID_ORG < 0){
        return;
    }
    db.UpdateOrganization(ui->lEdNameOrg->text(),ui->lEdBank->text(),ui->lEdAcc->text(),ID_ORG);//обновление записи  
    Refresh_Organization(); //обновление таблицы
    Mode("org_default");
}
void AdminWindow::sl_DeleteOrg()
{
    if (ID_ORG < 0){
        return;
    }

    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    QString err;
    err = db.DeleteOrg(ID_ORG);

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
    }
}

void AdminWindow::sl_CancelHome()
{
    Mode("home_default");
}

void AdminWindow::sl_DeleteHome()
{


    if (ID_HOME < 0){
        return;
    }

    if ((QMessageBox::question(this,trUtf8("ВНИМАНИЕ УДАЛЕНИЕ"),
                               trUtf8("Действительно хотите удалить?"),
                               QMessageBox::Yes | QMessageBox::No))==QMessageBox::No){
        return;
    }

    QString err;
    err = db.DeleteHome(ID_HOME);

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

    if (ID_HOME < 0){
        return;
    }
    db.UpdateHome(ID_HOME,ui->lEdHome->text());
    Refresh_Home();
    Mode("home_default");
}
