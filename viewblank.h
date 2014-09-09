#ifndef VIEWBLANK_H
#define VIEWBLANK_H

#include <QDialog>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebView>
#include <QWebFrame>
#include <QStringList>
#include <QToolButton>
#include <QBitmap>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QDesktopServices>
#include "parser_blank.h"

class ViewBlank : public QDialog
{
    Q_OBJECT
private:
    QVBoxLayout *layout;
    QHBoxLayout *btn_layout;
    QWebView *web;
    int r_year, r_month;
    int idOrg, idHome;
    parser_blank test_p;
    QStringList strL_page;
    int PageView;
public:
    explicit ViewBlank(QWidget *parent = 0);
    ~ViewBlank();
    void setDate(int year, int month);
    void setInfo(int id_home, int id_org);
    void generate();
    void open_blank(QString dir);

signals:
    
public slots:
private slots:
    void Print();
    void Pdf();
    void Next();
    void Preview();
    void First_Page();
    void Last_Page();
    void sl_Test();
    
};

#endif // VIEWBLANK_H
