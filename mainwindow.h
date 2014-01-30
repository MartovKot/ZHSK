#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QProgressBar>

#include "adminwindow.h"
#include "operwindow.h"
#include "apartament.h"
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
    Apartament Apartament_Share;
    void setVersion(QString ver);

private:
    Ui::MainWindow *ui;
    OperWindow *OperWin;
    AdminWindow *AdmWin;

    ViewBlank *VwBlank;
    void Install_param();
    int OrganiztionID, HomeID ;
    BD db;
    QString CurentDirArhiv;
    QString Version;


private slots:
    void Admin_mod();
    void Oper_mod();
    void Print_mod();
    void Refresh_win();
    void sl_ArhivKvit();  //Архив квитанций
    void sl_setCurDirArh(QString CurDir);
    void sl_OpenArhiv();


    void on_pBtnPrint_clicked();
};

#endif // MAINWINDOW_H
