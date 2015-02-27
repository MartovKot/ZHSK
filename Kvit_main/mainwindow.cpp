#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);

    AdmWin = nullptr;
    OperWin = nullptr;

    Refresh_win();

    ui->statusBar->showMessage(db.is_DatabaseVersoin());

    QFile styleFile(":/style.css");
    if( styleFile.open(QFile::ReadOnly) ) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
    }else{
        qDebug() << "Style can't be loaded.";
    }

    MainWindow::connect(ui->pBtnAdmin,SIGNAL(clicked()),this,SLOT(Admin_mod()));
    MainWindow::connect(ui->pBtnOper,SIGNAL(clicked()),this,SLOT(Oper_mod()));
    MainWindow::connect(ui->pBtnPrint,SIGNAL(clicked()),this,SLOT(Print_mod()));
    MainWindow::connect(ui->pBtn_ArhivBlank,SIGNAL(clicked()),this,SLOT(sl_ArhivKvit()));

}

MainWindow::~MainWindow()
{
    delete ui;
//    delete VwBlank;
}

void MainWindow::Refresh_win()
{
    ui->cmBx_Home->setModel(db.Model("homes"));
    ui->cmBx_Org->setModel(db.Model("organiz"));
}

void MainWindow::Admin_mod()
{
    if(AdmWin == nullptr){
        AdmWin = new AdminWindow(this);
        AdmWin->setPalette(this->palette());
        connect(AdmWin,SIGNAL(finished(int)),SLOT(sl_AdminWinClose()));
    }
    AdmWin->show();
}

void MainWindow::Oper_mod()
{
    if (OperWin == nullptr){
        OperWin = new OperWindow(this);
        OperWin->setPalette(this->palette());

        connect(OperWin,SIGNAL(finished(int)),SLOT(sl_OperWinClose()));
    }

    if (isIdSelectOrganiztion() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не выбрана организация"),QMessageBox::Ok);
        return;
    }
    if (isIdSelectHome() == -1){
        QMessageBox::warning(this,trUtf8("Не заполнены поля"),
                             trUtf8("Не выбран дом"),QMessageBox::Ok);
        return;
    }

    OperWin->set_parametr(isIdSelectOrganiztion(),isIdSelectHome());
    OperWin->setWindowTitle(ui->pBtnOper->text());
    OperWin->show();
}

void MainWindow::Print_mod()
{
    VwBlank = new ViewBlank(this);

    QList<int> list_id_apartament; //Список ид квартир

    list_id_apartament = db.is_ListIdApartament(isIdSelectOrganiztion(), isIdSelectHome());

    QProgressDialog progress(trUtf8("Идёт формирование..."), trUtf8("Отмена"), 0, list_id_apartament.size()-1, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Расчёт"));


    QDate date;
    date.setDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    progress.show();
    for(int i = 0;i<list_id_apartament.size();i++){
        progress.setValue(i);
        qApp->processEvents();
        if (progress.wasCanceled())
            break;
        db.CreditedOfService(list_id_apartament.at(i),date);// Формирование данных по i той квартире
    }
    progress.setValue(list_id_apartament.size()-1);

    //-------------------------------------------------------------------------------------------
//    db.DataProcessing(OrganiztionID, HomeID, month, year);
    //-------------------------------------------------------------------------------------------

    VwBlank->setDate(QDate::currentDate().year(),QDate::currentDate().month());
    VwBlank->setInfo(isIdSelectHome(),isIdSelectOrganiztion());
    VwBlank->generate();
    VwBlank->show();
}

void MainWindow::sl_OpenArhiv()
{
    VwBlank = new ViewBlank;
    connect(VwBlank,SIGNAL(rejected()),SLOT(sl_test()));
    VwBlank->open_blank(CurentDirArhiv);
    VwBlank->open();
}

void MainWindow::sl_test()
{
//    delete VwBlank;
//    VwBlank = nullptr;
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

int MainWindow::isIdSelectHome()
{
    int row;
    int id_home = -1;

    row = ui->cmBx_Home->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Home->model()->index(row, 1);
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

int MainWindow::isIdSelectOrganiztion()
{
    int row;
    int id_org = -1;

    row = ui->cmBx_Org->currentIndex();
    if (row != -1){
        QModelIndex index = ui->cmBx_Org->model()->index(row, 1);
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

void MainWindow::sl_OperWinClose()
{
    Refresh_win();
    delete OperWin;
    OperWin  = nullptr;
}

void MainWindow::sl_AdminWinClose()
{
    Refresh_win();
    delete AdmWin;
    AdmWin  = nullptr;
}
