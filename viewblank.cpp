#include "viewblank.h"
#include <QProcess>

ViewBlank::ViewBlank(QWidget *parent) :
    QDialog(parent)
{
    setWindowIcon(parent->windowIcon());
    setWindowTitle(trUtf8("Печать"));
    layout = new QVBoxLayout;
    btn_layout = new QHBoxLayout;
    web = new QWebView(this);

//    connect(this,SIGNAL(finished(int)),SLOT(sl_Test()));  ///!!!!!!!!!!!!!!!!!!!!!!


    QToolButton *btn_print = new QToolButton;
    QToolButton *btn_pdf = new QToolButton;
    QToolButton *btn_next = new QToolButton;
    QToolButton *btn_preview = new QToolButton;
    QToolButton *btn_end = new QToolButton;
    QToolButton *btn_home = new QToolButton;

    connect(btn_print,SIGNAL(clicked()),this,SLOT(Print()));
    connect(btn_next,SIGNAL(clicked()),this,SLOT(Next()));
    connect(btn_preview,SIGNAL(clicked()),this,SLOT(Preview()));
    connect(btn_home,SIGNAL(clicked()),this,SLOT(First_Page()));
    connect(btn_end,SIGNAL(clicked()),this,SLOT(Last_Page()));
    connect(btn_pdf,SIGNAL(clicked()),this,SLOT(Pdf()));

    btn_next->setText(trUtf8("&Следуюущий"));
    QPixmap pixmap(":/ico/forward.ico");
    btn_next->setIcon(QIcon(pixmap));
    btn_next->setIconSize(QSize(35,35));
    btn_next->setMask(btn_next->icon().pixmap(40,40).mask());

    btn_preview->setText(trUtf8("&Предыдущий"));
    pixmap.load(":/ico/previous.ico");
    btn_preview->setIcon(QIcon(pixmap));
    btn_preview->setIconSize(QSize(35,35));
    btn_preview->setMask(btn_preview->icon().pixmap(40,40).mask());

    btn_print->setText(trUtf8("&Печать"));
    pixmap.load(":/ico/print3.ico");
    btn_print->setIcon(QIcon(pixmap));
    btn_print->setIconSize(QSize(35,35));
    btn_print->setMask(btn_print->icon().pixmap(40,40,QIcon::Active,QIcon::On).mask());

    btn_pdf->setText(trUtf8("&PDF"));
    pixmap.load(":/ico/pdf.ico");
    btn_pdf->setIcon(QIcon(pixmap));
    btn_pdf->setIconSize(QSize(35,35));
    btn_pdf->setMask(btn_print->icon().pixmap(40,40,QIcon::Active,QIcon::On).mask());

    pixmap.load(":/ico/left_end.ico");
    btn_home->setIcon(QIcon(pixmap));
    btn_home->setIconSize(QSize(35,35));
    btn_home->setMask(pixmap.scaled(42,39).mask());

    pixmap.load(":/ico/right_end.ico");
    btn_end->setIcon(QIcon(pixmap));
    btn_end->setIconSize(QSize(35,35));
    btn_end->setMask(btn_end->icon().pixmap(40,40,QIcon::Active,QIcon::On).mask());

    btn_layout->addStretch();
    btn_layout->addWidget(btn_home);
    btn_layout->addWidget(btn_preview);
    btn_layout->addWidget(btn_print);
    btn_layout->addWidget(btn_pdf);
    btn_layout->addWidget(btn_next);
    btn_layout->addWidget(btn_end);
    btn_layout->addStretch();

    layout->addWidget(web);
    layout->addLayout(btn_layout);

    setLayout(layout);
}

ViewBlank::~ViewBlank()
{

}

void ViewBlank::setDate(int year, int month)
{
    r_year = year;
    r_month = month;
}
void ViewBlank::setInfo(int id_home, int id_org)
{
    idHome = id_home;
    idOrg = id_org;
}
void ViewBlank::generate()
{
    test_p.setDate(r_year,r_month,idHome,idOrg);
    test_p.generating();
    strL_page = test_p.OutStrL();
    PageView = 0;
    if(!strL_page.isEmpty()){
        web->setHtml(strL_page.first());
    }
}
void ViewBlank::open_blank(QString dir)
{
    test_p.setDir(dir);

    strL_page = test_p.OutStrL();
    PageView = 0;
    if(!strL_page.isEmpty()){
        web->setHtml(strL_page.first());
    }
}


void ViewBlank::Print()
{
    QPrinter printer;
    printer.setFromTo(1 ,strL_page.count());
    QPrintDialog printDialog(&printer,this);

    qDebug() << printDialog.exec();
    if  (printDialog.exec()){
        printer.setPageSize(QPrinter::A4);

        QPainter painter(&printer);
        QWebView view;
        QWebPage *page;
        QWebFrame *frame;
        if(     printer.fromPage() <= 0
                || printer.toPage() <= 0
                || printer.toPage() > strL_page.count()){       //устанавливаем страницы печати с первой по последнюю.
            printer.setFromTo(1,strL_page.count());
        }

        for (int i = printer.fromPage(); (i <= printer.toPage() && i <= strL_page.count()); i++) {
            view.setHtml(strL_page.at(i-1));
            page = view.page();
            page->setViewportSize(QSize(800,1200));
            frame = page->mainFrame();
            frame->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
            frame->render(&painter);
            if (i <= strL_page.count() && i  < printer.toPage())
               printer.newPage();
            delete page;
//            qDebug() << "==6==" << frame;
        }

    }else{
        qDebug() << "printer error";
    }
}

void ViewBlank::Pdf()
{

    QPrinter printer/*(QPrinter::HighResolution)*/;
    printer.setPageSize(QPrinter::A4);
    printer.setFromTo(1 ,strL_page.count());
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("output.pdf");


    QPainter painter(&printer);
    QWebView view;
    QWebPage *page;
    QWebFrame *frame;
    if(printer.fromPage() <= 0 || printer.toPage() <= 0 || printer.toPage() > strL_page.count()){       //устанавливаем страницы печати с первой по последнюю.
        printer.setFromTo(1,strL_page.count());
    }

    for (int i = printer.fromPage(); (i <= printer.toPage() && i <= strL_page.count()); i++) {
        view.setHtml(strL_page.at(i-1));
        page = view.page();
        int coef = 1;
        int x = 780*coef;
        int y = 1200*coef;
        page->setViewportSize(QSize(x,y));
        frame = page->mainFrame();
        frame->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
        frame->render(&painter);
        if (i <= strL_page.count() && i  < printer.toPage())
           printer.newPage();
        delete page;
    }

    QDesktopServices::openUrl(QUrl("output.pdf"));

}


void ViewBlank::Next()
{
    if(strL_page.isEmpty()){
        return;
    }

    if(PageView >= 0 && PageView<strL_page.size()-1){
        PageView++;
        web->setHtml(strL_page.at(PageView));
    }
}

void ViewBlank::Preview()
{
    if(strL_page.isEmpty()){
        return;
    }

    if(PageView > 0 && PageView < strL_page.size()){
        PageView--;
        web->setHtml(strL_page.at(PageView));
    }
}

void ViewBlank::Last_Page()
{
    if(strL_page.isEmpty()){
        return;
    }

    PageView = strL_page.size()-1;
    web->setHtml(strL_page.at(PageView));

}

void ViewBlank::First_Page()
{
    if(strL_page.isEmpty()){
        return;
    }
    PageView = 0;
    web->setHtml(strL_page.at(PageView));
}

//void ViewBlank::sl_Test()
//{
//    qDebug() << "TEST - TEST";
//}
