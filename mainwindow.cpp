#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//&&&&&
    AdmWin = NULL;
    OperWin = NULL;
    MainWindow::connect(ui->pBtnAdmin,SIGNAL(clicked()),this,SLOT(Admin_mod()));
    MainWindow::connect(ui->pBtnOper,SIGNAL(clicked()),this,SLOT(Oper_mod()));
    MainWindow::connect(ui->pBtnPrint,SIGNAL(clicked()),this,SLOT(Print_mod()));
    MainWindow::connect(ui->pBtnRefresh,SIGNAL(clicked()),this,SLOT(Refresh_win()));
    MainWindow::connect(ui->pBtn_ArhivBlank,SIGNAL(clicked()),this,SLOT(sl_ArhivKvit()));

    ui->centralWidget->setLayout(ui->verticalLayout);

    Refresh_win();
    OrganiztionID = -1;
    HomeID = -1;
    ui->statusBar->showMessage(db.is_DatabaseVersoin());
    QFile styleFile(":/style.css");
    if( styleFile.open(QFile::ReadOnly) ) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
    }else{
        qDebug() << "Style can't be loaded.";
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Refresh_win()
{
    ui->cmBx_Home->setModel(db.Model("homes"));
    ui->cmBx_Org->setModel(db.Model("organiz"));
}

void MainWindow::Admin_mod()
{
    if(AdmWin == NULL){
        AdmWin = new AdminWindow(this);
        AdmWin->setPalette(this->palette());
    }

    AdmWin->show();
}

void MainWindow::Oper_mod()
{
    Install_param();
    if (OperWin == NULL){
        OperWin = new OperWindow(this);
    }


    OperWin->set_parametr(OrganiztionID,HomeID);
    OperWin->setWindowTitle(ui->pBtnOper->text());
    OperWin->show();
}

void MainWindow::Install_param()
{

    int row;

    row = ui->cmBx_Org->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org->model()->index(row, 1);
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

    row = ui->cmBx_Home->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home->model()->index(row, 1);
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
}

void MainWindow::Print_mod()
{
    VwBlank = new ViewBlank(this);

    QList<int> list_id_apartament; //Список ид квартир

    Install_param();
    list_id_apartament = db.is_ListIdApartament(OrganiztionID, HomeID);

    QProgressBar *PB = new QProgressBar();
    PB->setWindowIcon(this->windowIcon());
    PB->setWindowTitle(trUtf8("Идёт формирование..."));
    PB->setMinimum(0);
    PB->setMaximum(list_id_apartament.size()-1);
    PB->show();
    //------------------------------------------------------------------------------------------
    for(int i = 0;i<list_id_apartament.size();i++){
        db.CreditedOfService(list_id_apartament.at(i),QDate::currentDate());// Формирование данных по i той квартире
        PB->setValue(i);
    }
    //-------------------------------------------------------------------------------------------
//    db.DataProcessing(OrganiztionID, HomeID, month, year);
    //-------------------------------------------------------------------------------------------
    delete PB;

    VwBlank->setDate(QDate::currentDate().year(),QDate::currentDate().month());
    VwBlank->setInfo(HomeID,OrganiztionID);
    VwBlank->generate();
    VwBlank->show();
}

void MainWindow::sl_OpenArhiv()
{
    VwBlank = new ViewBlank(this);
    VwBlank->open_blank(CurentDirArhiv);
    VwBlank->show();
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

void MainWindow::on_pBtnPrint_clicked()
{

}
