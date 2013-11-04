#ifndef PARSER_BLANK_H
#define PARSER_BLANK_H
#include <QObject>

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>

#include "bd.h"

class parser_blank :public QWidget
{
    Q_OBJECT
public:
    parser_blank(QWidget *parent=0);
    QString isfirstfile();
    QString isnextfile();
    void setDate( int year, int month, int id_home, int id_org);
    void setDir(QString str_dirFolder);
    void generating();
    QStringList OutStrL();

private:
    struct ConfigData {
        int Org_id, Home_id, month, year;
    } ConfData;

    QString str_folder;  //каталог для создания отчётов
    QString str_folder_arhiv;
    BD db;
    void creat_blank(QString name,int id_app,int year,int month);
    QString process_usluga(QString str_in_usl, int id_app,int year,int month);
    QString process_schet(QString str_in_sch, int id_app,int year,int month);
    QString process_main(QString str_in, int id_app);
signals:
//    void sgn_FolderAlreadyPresent();

};

#endif // PARSER_BLANK_H
