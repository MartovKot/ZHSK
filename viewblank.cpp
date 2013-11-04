#include "viewblank.h"

ViewBlank::ViewBlank(QWidget *parent) :
    QDialog(parent)
{
    setWindowIcon(parent->windowIcon());
    setWindowTitle(trUtf8("Печать"));
    layout = new QVBoxLayout;
    btn_layout = new QHBoxLayout;
//    connect(&test_p,SIGNAL(sgn_FolderAlreadyPresent()),SLOT(sl_ReWriteDir())); //Повторное формирование квитанции

    QToolButton *btn_print = new QToolButton;
    QToolButton *btn_next = new QToolButton;
    QToolButton *btn_preview = new QToolButton;
    QToolButton *btn_end = new QToolButton;
    QToolButton *btn_home = new QToolButton;

    connect(btn_print,SIGNAL(clicked()),this,SLOT(Print()));
    connect(btn_next,SIGNAL(clicked()),this,SLOT(Next()));
    connect(btn_preview,SIGNAL(clicked()),this,SLOT(Preview()));
    connect(btn_home,SIGNAL(clicked()),this,SLOT(First_Page()));
    connect(btn_end,SIGNAL(clicked()),this,SLOT(Last_Page()));

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
    btn_layout->addWidget(btn_next);
    btn_layout->addWidget(btn_end);
    btn_layout->addStretch();

    layout->addWidget(&web);
    layout->addLayout(btn_layout);

    setLayout(layout);
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
        web.setHtml(strL_page.first());
    }
}
void ViewBlank::open_blank(QString dir)
{
    test_p.setDir(dir);

    strL_page = test_p.OutStrL();
//    qDebug()<<strL_page.count();
    PageView = 0;
    if(!strL_page.isEmpty()){
        web.setHtml(strL_page.first());
    }
}


void ViewBlank::Print()
{
    QPrinter printer;
    printer.setFromTo(1 ,strL_page.count());
    QPrintDialog printDialog(&printer);
    if  (printDialog.exec()){
//        qDebug()<<"TEST";
        printer.setPageSize(QPrinter::A4);

        QPainter painter(&printer);
        QWebView view;
        QWebPage *page;
        QWebFrame *frame;
        qDebug()<<strL_page.count()<<" "<<printer.fromPage()<<" "<<printer.toPage();
        if(printer.fromPage() == 0 && printer.toPage() == 0){
            printer.setFromTo(1,strL_page.count());
        }
        qDebug()<<strL_page.count()<<" "<<printer.fromPage()<<" "<<printer.toPage();

        for (int i = printer.fromPage(); (i <= printer.toPage() && i <= strL_page.count()); i++) {
//            qDebug()<<"test in " << i;
            view.setHtml(strL_page.at(i-1));
            page = view.page();
            page->setViewportSize(QSize(800,1200));
            frame = page->mainFrame();
            frame->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
            frame->render(&painter);
            if (i + 1 < strL_page.count() && i + 1  < printer.toPage())
                printer.newPage();
            delete page;
        }

    }
}

void ViewBlank::Next()
{
    if(strL_page.isEmpty()){
        return;
    }

    if(PageView >= 0 && PageView<strL_page.size()-1){
        PageView++;
        web.setHtml(strL_page.at(PageView));
    }
}

void ViewBlank::Preview()
{
    if(strL_page.isEmpty()){
        return;
    }

    if(PageView > 0 && PageView<strL_page.size()){
        PageView--;
        web.setHtml(strL_page.at(PageView));
    }
}

void ViewBlank::Last_Page()
{
    if(strL_page.isEmpty()){
        return;
    }

    PageView = strL_page.size()-1;
    web.setHtml(strL_page.at(PageView));

}

void ViewBlank::First_Page()
{
    if(strL_page.isEmpty()){
        return;
    }
    PageView = 0;
    web.setHtml(strL_page.at(PageView));
}
