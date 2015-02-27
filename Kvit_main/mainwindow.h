#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>

#include <QImageReader>
#include <QProgressDialog>

#include "adminwindow.h"
#include "operwindow.h"
#include "viewblank.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setVersion(QString ver);

private:
    Ui::MainWindow *ui;
    OperWindow *OperWin;
    AdminWindow *AdmWin;
    ViewBlank *VwBlank;
    BD db;
    QString CurentDirArhiv;
    QString Version;

    int isIdSelectHome();
    int isIdSelectOrganiztion();

private slots:
    void Admin_mod();
    void Oper_mod();
    void Print_mod();
    void Refresh_win();
    void sl_ArhivKvit();  //Архив квитанций
    void sl_setCurDirArh(QString CurDir);
    void sl_OpenArhiv();
    void sl_OperWinClose();
    void sl_AdminWinClose();
    void sl_test();
};

#endif // MAINWINDOW_H
