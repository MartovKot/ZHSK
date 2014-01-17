#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonArray>

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



    m_url = "https://api.github.com/repos/MartovKot/ZHSK/releases";

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("10.62.0.9");
    proxy.setPort(3128);
    QNetworkProxy::setApplicationProxy(proxy);

    m_request.setUrl(m_url);
    m_manager = new QNetworkAccessManager(this);
    m_manager->setProxy(proxy);
    m_reply = m_manager->get(m_request);

    QObject::connect(m_manager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(finishedSlot(QNetworkReply*)));

}

void MainWindow::finishedSlot(QNetworkReply*)
{
    // Reading attributes of the reply
       // e.g. the HTTP status code
       QVariant statusCodeV = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
       qDebug() << statusCodeV;
       // Or the target URL if it was a redirect:
//       QVariant redirectionTargetUrl = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
//       qDebug() << redirectionTargetUrl;
       // see CS001432 on how to handle this

       // no error received?
        if (m_reply->error() == QNetworkReply::NoError)
        {
            QByteArray bytes = m_reply->readAll();  // bytes
            QString string(bytes); // string

            QJsonDocument m_jdoc;
            m_jdoc = QJsonDocument::fromJson(string.toUtf8());
            qint64 last_time = 0;
            QString path = "";
            QStringList path_list;
            for (int i = 0;i < m_jdoc.array().size();i++){
                if (QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch() > last_time){
                    last_time = QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch();
                    path += "https://github.com/MartovKot/ZHSK/releases";
                    path += "/download/";
                    path += m_jdoc.array().takeAt(i).toObject().value("tag_name").toString();
                    path += "/";
                    for (int j = 0; j < m_jdoc.array().takeAt(i).toObject().value("assets").toArray().size(); j++){
                        path_list << path + m_jdoc.array().takeAt(i).toObject().value("assets").toArray().takeAt(i).toObject().value("name").toString();
                    }
                }
            }
            HttpWin = new HttpWindow;
            HttpWin->show();
            HttpWin->setUrl(path_list.at(0));
       }
       // Some http error received
       else
       {
           qDebug()<< m_reply->errorString();
           // handle errors here
       }

       delete m_reply;
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
        db.CreditedOfService(QDate::currentDate().month(),
                             QDate::currentDate().year(),
                             list_id_apartament.at(i));// Формирование данных по i той квартире
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
