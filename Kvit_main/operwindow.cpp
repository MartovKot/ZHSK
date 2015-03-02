#include "operwindow.h"
#include "ui_operwindow.h"
#include "myitemdelegate.h"


OperWindow::OperWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperWindow)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout); //указываем главный  слой

    connect(ui->pBtn_AddPayment,SIGNAL(clicked()),SLOT(sl_addPayment()));
    connect(ui->pBtn_AddPayment,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
    connect(ui->pBtn_EditPokazanie,SIGNAL(clicked()),SLOT(sl_EditPokazanie()));
    connect(ui->pBtn_EditPokazanie,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
    connect(ui->pBtn_DeletePayment,SIGNAL(clicked()),SLOT(sl_DeletePayment()));
    connect(ui->pBtn_DeletePayment,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
    connect(ui->tBtn_Calendar,SIGNAL(clicked()),SLOT(sl_Calendar()));

    connect(ui->tBtn_ApartLeftEnd,SIGNAL(clicked()),SLOT(sl_ApartFirst()));
    connect(ui->tBtn_ApartLeft,SIGNAL(clicked()),SLOT(sl_ApartPrevious()));
    connect(ui->tBtn_ApartRight,SIGNAL(clicked()),SLOT(sl_ApartNext()));
    connect(ui->tBtn_ApartRightEnd,SIGNAL(clicked()),SLOT(sl_ApartLast()));

    connect(ui->dEd_Count,SIGNAL(editingFinished()),SLOT(sl_RefreshFull()));
    connect(ui->dEd_Count,SIGNAL(customContextMenuRequested(QPoint)),SLOT(sl_RefreshFull()));
    connect(ui->cmBx_NumApartanent,SIGNAL(activated(int)),SLOT(sl_RefreshFull()));
    connect(ui->pBtn_NewCounter,SIGNAL(clicked()),SLOT(sl_NewCounter()));

    QRegExpValidator *dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),this); //валидатор для чисел
    ui->lEd_Sum->setValidator(dv);
    ui->dEd_Count->setDate(QDate::currentDate());
    ui->dEd_Payment->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));

    QPixmap pixmap(":/ico/base_calendar_32.png");
    ui->tBtn_Calendar->setIcon(QIcon(pixmap));
    ui->tBtn_Calendar->setMask(pixmap.mask());

    ui->groupBox->setLayout(ui->horizontalLayout_10);
    ui->groupBox_2->setLayout(ui->horizontalLayout_5);



}

OperWindow::~OperWindow()
{
    delete ui;
}


void OperWindow::Refresh_tblVPayment(int ApartamenID)
{
    Table_Payment t_payment;
    ui->tblV_Payment->setModel(t_payment.ModelPayment(ApartamenID));
    ui->tblV_Payment->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Payment->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_Payment->horizontalHeader()->setSectionResizeMode( 3, QHeaderView::Interactive);
}

void OperWindow::sl_addPayment()
{
    QString Payment;
    int day, month,year;

    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    Payment = ui->lEd_Sum->text();
    if (Payment == ""){
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
    t_payment.add_line(QString::number(apartment.getId()),QString::number(year),QString::number(month),QString::number(day),Payment);

    Refresh_tblVPayment(apartment.getId());
}

//------------------------------------------------------------------------------------------------------------
void OperWindow::Refresh_tblVCount()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    ui->tblV_Count->setModel(db.ModelPokazanie(apartment.getId(),
                                 ui->dEd_Count->date().month(),
                                 ui->dEd_Count->date().year()));
    db.new_pokazanie(apartment.getId(),                     //новое показание на след месяц
                     ui->dEd_Count->date().month(),
                     ui->dEd_Count->date().year());
    ui->tblV_Count->hideColumn(0);
    ui->tblV_Count->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Count->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblV_Count->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);

}
//-----------------------------------------------------------------------------------------------------------

void OperWindow::set_parametr(int id_org, int id_home)
{
    Organization organization;
    organization.setId(id_org);
    Home home;
    home.setId(id_home);

    HomeID = id_home;
    OrganizationID = id_org;

    ui->lbl_home->setText("<font color=blue>"+home.getName()+"</font>");
    ui->lbl_organization->setText(organization.getName());
    Apartment apartment;
    ui->cmBx_NumApartanent->setModel(apartment.ModelAllApartment(id_home,id_org));

}

void OperWindow::sl_EditPokazanie()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    //Новое окно редактирования
    dlg = new QDialog(this);

    dlg->setModal(true);
    dlg->setPalette(this->palette());
    dlg->setWindowTitle(trUtf8("Редактирование показаний"));

    QVBoxLayout *main_lay = new QVBoxLayout;
    QHBoxLayout *lay_1 = new QHBoxLayout;
    tbl = new TableViewPokazanie(this);
    MyItemDelegate * dDeleg = new MyItemDelegate ();
    SqlQueryEditModel *edModel;

    edModel = db.ModelEditPokazanie(apartment.getId(),ui->dEd_Count->date().month(),ui->dEd_Count->date().year());

    tbl->setModel(edModel);
    tbl->setItemDelegate(dDeleg);

    connect(tbl,SIGNAL(closing()),dlg,SLOT(close()));
    connect(dlg,SIGNAL(finished(int)),SLOT(Refresh_tblVCount()));
    connect(dlg,SIGNAL(finished(int)),SLOT(sl_RefreshLabel()));

    tbl->horizontalHeader()->setStretchLastSection(false);

    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    if(tbl->model()->columnCount()>3){
        tbl->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    }

    lay_1->addWidget(tbl);
    main_lay->addLayout(lay_1);

    dlg->setLayout(main_lay);
    dlg->resize(main_lay->sizeHint().width()+50,main_lay->sizeHint().height());
    dlg->show();

}

void OperWindow::keyPressEvent(QKeyEvent *event)
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

void OperWindow::sl_DeletePayment()
{
    int row;
    int month,year,day;

    row = ui->tblV_Payment->currentIndex().row();
    if (row<0){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не выбрана оплата"),QMessageBox::Ok);
        return;
    }

    day = ui->tblV_Payment->model()->index(row,0).data().toInt();
    month = ui->tblV_Payment->model()->index(row,1).data().toInt();
    year = ui->tblV_Payment->model()->index(row,2).data().toInt();

    if(QMessageBox::question(this,trUtf8("Удаление"),
                         trUtf8("Удалить эту запись?\n")+QString::number(day)+" "+
                         QDate::longMonthName(month)+" "+QString::number(year)+trUtf8(" года"),
                    QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok){
        QString err;
        Table_Payment t_payment;
        Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());
        err = t_payment.delete_Payment(
                    apartment.getId(),
                    ui->tblV_Payment->model()->index(row,2).data().toInt(),
                    ui->tblV_Payment->model()->index(row,1).data().toInt(),
                    ui->tblV_Payment->model()->index(row,0).data().toInt());
        if (err == ""){
            QMessageBox::information(this,trUtf8("Успех"),
                                 trUtf8("Удаление прошло успешно \n")+err,QMessageBox::Ok);
            Refresh_tblVPayment(apartment.getId());
        }else{
            QMessageBox::critical(this,trUtf8("Ошибка"),
                                 trUtf8("Удаление не произошло \n")+err,QMessageBox::Ok);
        }
    }
}

void OperWindow::sl_Calendar()
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

void OperWindow::sl_RefreshLabel() //обновление выводяшейся оплаты и долга
{

    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());


    int month = ui->dEd_Count->date().month();
    int year = ui->dEd_Count->date().year();
    DateOfUnixFormat date_calc(year,month,1);

    if (ui->dEd_Count->date() == QDate::currentDate()){ //Расчёт производим только за текущий месяц
        calculation.CreditedOfService(apartment.getId(),date_calc);
    }
    ui->lblInPayment->setText(QString::number(calculation.AmountToPay(apartment.getId(),date_calc.Second())));
    ui->lblDolg->setText(db.is_Debt(apartment.getId(),date_calc));
}

void OperWindow::sl_ApartFirst()
{
    ui->cmBx_NumApartanent->setCurrentIndex(0);
    sl_RefreshFull();
}

void OperWindow::sl_ApartLast()
{
    ui->cmBx_NumApartanent->setCurrentIndex(ui->cmBx_NumApartanent->count()-1);
    sl_RefreshFull();
}

void OperWindow::sl_ApartNext()
{
    int index;
    index = ui->cmBx_NumApartanent->currentIndex();
    if(index < ui->cmBx_NumApartanent->count()-1){
        index++;
    }
    ui->cmBx_NumApartanent->setCurrentIndex(index);
    sl_RefreshFull();
}

void OperWindow::sl_ApartPrevious()
{
    int index;

    index = ui->cmBx_NumApartanent->currentIndex();
    if(index > 0){
        index--;
    }
    ui->cmBx_NumApartanent->setCurrentIndex(index);
    sl_RefreshFull();
}

void OperWindow::sl_RefreshFull()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());

    Refresh_tblVPayment(apartment.getId());
    Refresh_tblVCount();
    sl_RefreshLabel();
    Refresh_lbl_Payer();

}

void OperWindow::sl_NewCounter()//вызывается когда происходит смена счётчика
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
    connect(dlg,SIGNAL(finished(int)),this,SLOT(sl_RefreshFull()));
    dlg->open();
}

void OperWindow::on_pBtn_NewCounterNext_clicked()
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
    dlg->set_IdPokazanie(db.new_pokazanie(id_counter,"0"));
    dlg->open();
}

void OperWindow::Refresh_lbl_Payer()
{
    Apartment apartment(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt());


    ui->lbl_Payer->setText(apartment.is_FIO_payer());
}

void OperWindow::on_pBtn_AddPayment_clicked()
{

}
