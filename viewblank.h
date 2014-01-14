#ifndef VIEWBLANK_H
#define VIEWBLANK_H

#include <QDialog>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebKitPlatformPlugin>
#include <QWebFrame>
#include <QStringList>
#include <QToolButton>
#include <QBitmap>
#include <QPrinter>
#include <QPrintDialog>
#include "parser_blank.h"

class ViewBlank : public QDialog
{
    Q_OBJECT
private:
    QVBoxLayout *layout;
    QHBoxLayout *btn_layout;
    QWebView web;
    int r_year, r_month;
    int idOrg, idHome;
    parser_blank test_p;
    QStringList strL_page;
    int PageView;
public:
    explicit ViewBlank(QWidget *parent = 0);
    void setDate(int year, int month);
    void setInfo(int id_home, int id_org);
    void generate();
    void open_blank(QString dir);


    
signals:
    
public slots:
private slots:
//    void sl_ReWriteDir();
    void Print();
    void Next();
    void Preview();
    void First_Page();
    void Last_Page();
    
};

#endif // VIEWBLANK_H
