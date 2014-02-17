#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTextCodec>
#include <QtNetwork>

#include "mainwindow.h"
#include "bd.h"
#include "logreport.h"
#include "updater.h"

#define VERSION "1.4.0"

static bool createConnection() //подключение к БД
{
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

    Updater upd;
    upd.setVersion(VERSION);
    upd.RunUpdate();

    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

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
    w.show();
    splash.finish(&w);

    return a.exec();
}
