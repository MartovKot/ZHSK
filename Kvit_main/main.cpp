#include <QApplication>
#include <QSplashScreen>
#include <QTextCodec>

#include "mainwindow.h"
#include "bd.h"
#include "logreport.h"
#include "updater.h"

#define VERSION "1.7.2"

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
    QApplication app(argc, argv);

    QPixmap pixmap(":/ico/splash.png");
    QSplashScreen splash(pixmap);
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

    Updater upd;   //Обновление ПО
    upd.setVersion(VERSION);
    upd.RunUpdate();

    BD bd;
    QString str = "SELECT version FROM version";
    QSqlQuery query;
    if (!query.exec(str)){
        qDebug()<<"Creat db";
        if (!bd.Create()){//Создаём структу
            QMessageBox::critical(new QWidget(),QObject::trUtf8("Ошибка"),
                                 QObject::trUtf8("Не создана структура БД"),QMessageBox::Ok);
            return app.exec();
        }
    }

    query.finish();
    out.logout(QObject::trUtf8("Проверка обновлений ..."));
    bd.UpdateDataBase();

    out.logout(QObject::trUtf8("Производятся установки"));
    str = "PRAGMA foreign_keys = true;";
    if (query.exec(str)){
        out.logout(QObject::trUtf8("Успешно"));
    }

    MainWindow w;
    QObject::connect(&upd,SIGNAL(s_run_update()),&w,SLOT(close()));
    w.setVersion(VERSION);

    out.logout(QObject::trUtf8("!!!! Начинаем работать !!!!!"));
    splash.close();
    w.show();
    splash.finish(&w);


    return app.exec();
}
