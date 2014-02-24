#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTextCodec>
#include <QtNetwork>

#include "mainwindow.h"
#include "bd.h"
#include "logreport.h"
#include "updater.h"

#define VERSION "1.5.1"

static bool createConnection() //подключение к БД
{
    /// Временный блок для востановления БД
//    QFile file("backup.qsl");
//    file.open(QIODevice::ReadOnly);
//    if (file.isReadable()){
//        QFile oldDB("kvitdb.qsl");

//        oldDB.open(QIODevice::ReadOnly);
//        if (oldDB.isReadable()){
//            if (oldDB.remove()){
//                file.copy("kvitdb.qsl");
//            }else{
//                qDebug() << "no delete "<< file.errorString();
//            }
//        }else{
//            qDebug()<< "no read old" << oldDB.errorString();
//        }
//    }else{
//        qDebug()<< "no read backup" << file.errorString();
//    }

//        qDebug() << "delete DB is " << oldDB.remove();
    //------------------------


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("kvitdb.qsl");
    LogReport out;
    out.setFileName("out.log");

    if (!db.isOpen())
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        out.logout(db.lastError().text());
        return false;
    }
    return true;

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/images/main.png");
    QSplashScreen splash(pixmap);
    splash.show();

    Updater upd;
    upd.setVersion(VERSION);
    upd.RunUpdate();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf-8"));
    LogReport out;
    out.setFileName("out.log");

    if (!createConnection()){
        out.logout(QObject::trUtf8("Нет соединения с БД"));
        QMessageBox::critical(new QWidget(),QObject::trUtf8("Ошибка"),
                             QObject::trUtf8("Нет соединения с БД"),QMessageBox::Ok);
        return -1;
    }
    out.logout(QObject::trUtf8("Соединение с базой установленно"));
    BD bd;
    QString str = "SELECT version FROM version";
    QSqlQuery query;
    if (!query.exec(str)){
        qDebug()<<"Creat db";
        bd.Create();//Создаём структу
    }

    out.logout(QObject::trUtf8("Производятся установки"));
    str = "PRAGMA foreign_keys = true;";
    if (query.exec(str)){
        out.logout(QObject::trUtf8("Успешно"));
    }

    query.finish();
    out.logout(QObject::trUtf8("Проверка обновлений ..."));
    bd.UpdateDataBase();

    MainWindow w;
    QObject::connect(&upd,SIGNAL(s_run_update()),&w,SLOT(close()));
    w.setVersion(VERSION);
    out.logout(QObject::trUtf8("!!!! Начинаем работать !!!!!"));
    splash.close();
    w.show();
    splash.finish(&w);

    return a.exec();
}
