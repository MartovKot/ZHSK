#include "editindicationscounter.h"
#include "ui_editindicationscounter.h"

EditIndicationsCounter::EditIndicationsCounter(int id, int m, int y, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditIndicationsCounter)
{
    ui->setupUi(this);
    tbl = new TableViewPokazanie(this);

    setLayout(ui->mainLay);

    dDeleg = new MyItemDelegate (this);
    Indications indications;
    edModel = indications.ModelEditPokazanie(id,m,y);

    tbl->setModel(edModel);
    tbl->setItemDelegate(dDeleg);
    connect(tbl,SIGNAL(closing()),SLOT(close()));
    connect(edModel,SIGNAL(sgn_EditPokazanie(int,QString)),SLOT(slEdit(int,QString)));
    connect(edModel,SIGNAL(sgn_RefreshModel(QAbstractTableModel*)),this,SLOT(slModelPokazanieHeaderData(QAbstractTableModel*)));
    slModelPokazanieHeaderData(edModel);



    tbl->horizontalHeader()->setStretchLastSection(false);
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    if(tbl->model()->columnCount()>3){
        tbl->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    }
    ui->mainLay->addWidget(tbl);

}

EditIndicationsCounter::~EditIndicationsCounter()
{
    delete ui;
}

void EditIndicationsCounter::slEdit(int id_pok, QString value)
{
    Indications indications(id_pok);
    indications.UpdateEnd(value.toInt());
}

void EditIndicationsCounter::slModelPokazanieHeaderData(QAbstractTableModel *t)
{
    t->setHeaderData(0, Qt::Horizontal, QObject::trUtf8("№"));
    t->setHeaderData(1, Qt::Horizontal, QObject::trUtf8("Счётчик"));
    t->setHeaderData(2, Qt::Horizontal, QObject::trUtf8("Показазия текущие"));
}



