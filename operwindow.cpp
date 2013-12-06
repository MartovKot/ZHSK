#include "operwindow.h"
#include "ui_operwindow.h"
#include "myitemdelegate.h"


OperWindow::OperWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperWindow)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout); //указываем главный  слой

    connect(ui->pBtn_AddBay,SIGNAL(clicked()),SLOT(sl_addBay()));
    connect(ui->pBtn_AddBay,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
    connect(ui->pBtn_EditPokazanie,SIGNAL(clicked()),SLOT(sl_EditPokazanie()));
    connect(ui->pBtn_EditPokazanie,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
    connect(ui->pBtn_DeleteBay,SIGNAL(clicked()),SLOT(sl_EditBay()));
    connect(ui->pBtn_DeleteBay,SIGNAL(clicked()),SLOT(sl_RefreshLabel()));
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
    ui->dEd_Bay->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    QPixmap pixmap(":/ico/base_calendar_32.png");
    ui->tBtn_Calendar->setIcon(QIcon(pixmap));
    ui->tBtn_Calendar->setMask(pixmap.mask());
}

OperWindow::~OperWindow()
{
    delete ui;
}


void OperWindow::Refresh_tblVBay(int ApartamenID)
{
    ui->tblV_Bay->setModel(t_bay.ModelBay(ApartamenID));
    ui->tblV_Bay->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Bay->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tblV_Bay->horizontalHeader()->setResizeMode( 3, QHeaderView::Interactive);
}

void OperWindow::sl_addBay()
{
    QString bay;
    int day, month,year,num;

    num = ui->cmBx_NumApartanent->model()->index(ui->cmBx_NumApartanent->currentIndex(), 1).data().toInt(); //ID квартиры

    bay = ui->lEd_Sum->text();
    if (bay == ""){
        return;
    }
    day = ui->dEd_Bay->date().day();
    month = ui->dEd_Bay->date().month();
    year = ui->dEd_Bay->date().year();
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

    t_bay.add_line(QString::number(num),QString::number(year),QString::number(month),QString::number(day),bay);

    Refresh_tblVBay(num);
}

//------------------------------------------------------------------------------------------------------------
void OperWindow::Refresh_tblVCount()
{
    int id_apartament;
    id_apartament = ui->cmBx_NumApartanent->model()->index(ui->cmBx_NumApartanent->currentIndex(), 1).data().toInt();
    ui->tblV_Count->setModel(db.ModelPokazanie(id_apartament,
                                 ui->dEd_Count->date().month(),
                                 ui->dEd_Count->date().year()));
    db.new_pokazanie(id_apartament,
                     ui->dEd_Count->date().month(),
                     ui->dEd_Count->date().year());
    ui->tblV_Count->hideColumn(0);
    ui->tblV_Count->horizontalHeader()->setStretchLastSection(false);
    ui->tblV_Count->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tblV_Count->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
}
//-----------------------------------------------------------------------------------------------------------

void OperWindow::set_parametr(int id_org, int id_home)
{
    HomeID = id_home;
    OrganizationID = id_org;
    ui->lbl_home->setText("<font color=blue>"+db.is_nameHome(id_home)+"</font>");
    ui->lbl_organization->setText(db.is_nameOrg(id_org));
    ui->cmBx_NumApartanent->setModel(db.ModelApartament(id_home,id_org));
}

void OperWindow::sl_EditPokazanie()
{
    int ApartamentID;// Получим id квартиры
    int row = ui->cmBx_NumApartanent->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_NumApartanent->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                ApartamentID = index.data().toInt();
            }else{
                QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                     trUtf8("Ошибка в поле номер квартиры"),QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                                 trUtf8("Ошибка в поле номер квартиры"),QMessageBox::Ok);
            return;
        }
    }else{
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Ошибка в поле номер квартиры"),QMessageBox::Ok);
        return;
    }

    //Новое окно редактирования
    dlg = new QDialog(this);

    dlg->setModal(true);
    dlg->setPalette(this->palette());
    dlg->setWindowTitle(trUtf8("Редактирование показаний"));

    QVBoxLayout *main_lay = new QVBoxLayout;
    QHBoxLayout *lay_1 = new QHBoxLayout;
    tbl = new TableViewPokazanie(this);
    MyItemDelegate * dDeleg = new MyItemDelegate ();
    SqlQueryEditModel *edModel = new SqlQueryEditModel(this);

    edModel = db.ModelEditPokazanie(ApartamentID,ui->dEd_Count->date().month(),ui->dEd_Count->date().year());

    tbl->setModel(edModel);
    tbl->setItemDelegate(dDeleg);

    connect(tbl,SIGNAL(closing()),dlg,SLOT(close()));
    connect(dlg,SIGNAL(finished(int)),SLOT(Refresh_tblVCount()));
    connect(dlg,SIGNAL(finished(int)),SLOT(sl_RefreshLabel()));

    tbl->horizontalHeader()->setStretchLastSection(false);
    tbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    tbl->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    tbl->horizontalHeader()->setResizeMode(3, QHeaderView::Interactive);

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
            ui->dEd_Bay->setFocus();
        }else if(focusWidget() == ui->dEd_Count){
            ui->cmBx_NumApartanent->setFocus();
        }else if(focusWidget() == ui->dEd_Bay){
            ui->lEd_Sum->setFocus();
        }else if(focusWidget() == ui->lEd_Sum){
            ui->pBtn_AddBay->click();
            ui->pBtn_EditPokazanie->setFocus();
        }
        break;
    default: break;
    }
}

void OperWindow::sl_EditBay()
{
    int row;
    int month,year,day;

    row = ui->tblV_Bay->currentIndex().row();
    if (row<0){
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             trUtf8("Не выбрана оплата"),QMessageBox::Ok);
        return;
    }

    day = ui->tblV_Bay->model()->index(row,0).data().toInt();
    month = ui->tblV_Bay->model()->index(row,1).data().toInt();
    year = ui->tblV_Bay->model()->index(row,2).data().toInt();

    if(QMessageBox::question(this,trUtf8("Удаление"),
                         trUtf8("Удалить эту запись?\n")+QString::number(day)+" "+
                         QDate::longMonthName(month)+" "+QString::number(year)+trUtf8(" года"),
                    QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Ok){
        QString err;
        err = db.delete_Bay(
                    db.is_idappart(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt()),
                    ui->tblV_Bay->model()->index(row,2).data().toInt(),
                    ui->tblV_Bay->model()->index(row,1).data().toInt(),
                    ui->tblV_Bay->model()->index(row,0).data().toInt());
        if (err == ""){
            QMessageBox::information(this,trUtf8("Успех"),
                                 trUtf8("Удаление прошло успешно \n")+err,QMessageBox::Ok);
            Refresh_tblVBay(db.is_idappart(HomeID,OrganizationID,ui->cmBx_NumApartanent->currentText().toInt()));
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
    connect(calendar,SIGNAL(clicked(QDate)),ui->dEd_Bay,SLOT(setDate(QDate)));
    connect(calendar,SIGNAL(clicked(QDate)),w,SLOT(close()));
    ui->lEd_Sum->setFocus();
    w->open();
}

void OperWindow::sl_RefreshLabel()
{
    int ApartamentID;
    int row = ui->cmBx_NumApartanent->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_NumApartanent->model()->index(row, 1);
        if (index.isValid()){
            if (index.data().canConvert(QVariant::Int)){
                ApartamentID = index.data().toInt();
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

    int day = QDate::currentDate().day();
    int month = ui->dEd_Count->date().month();
    int year = ui->dEd_Count->date().year();

    QDate date_calc;
    date_calc.setDate(year,month,day);
    if (date_calc.daysTo(QDate::currentDate()) == 0){ //Расчёт производим только за текущий месяц
        db.CreditedOfService(ui->dEd_Count->date().month(),
                             ui->dEd_Count->date().year(),
                             ApartamentID);
    }
    ui->lblInBay->setText(QString::number(db.AmountToPay(ApartamentID,ui->dEd_Count->date().month(),
                                                         ui->dEd_Count->date().year())));

    ui->lblDolg->setText(db.is_Debt(ApartamentID,ui->dEd_Count->date().month(),ui->dEd_Count->date().year()));
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
    int num = ui->cmBx_NumApartanent->model()->index(ui->cmBx_NumApartanent->currentIndex(), 1).data().toInt();
    Refresh_tblVBay(num);
    Refresh_tblVCount();
    sl_RefreshLabel();
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
    connect(dlg,SIGNAL(finished(int)),this,SLOT(Refresh_tblVCount()));
    dlg->open();
}
