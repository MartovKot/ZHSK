#include "newcounter.h"
#include "ui_newcounter.h"


NewCounter::NewCounter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCounter)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    QRegExpValidator *only_number;
    only_number = new QRegExpValidator(QRegExp("[0-9]+"),this);   //валидатор для цифр
    ui->lEdCounter->setValidator(only_number);
    this->setWindowTitle(trUtf8("Новый счётчик"));

    connect(ui->pBtn_Cancel,SIGNAL(clicked()),SLOT(close()));
    connect(ui->pBtn_Ok,SIGNAL(clicked()),SLOT(sl_Ok()));
    this->adjustSize();

}

NewCounter::~NewCounter()
{
    delete ui;
}

void NewCounter::set_IdPokazanie(int id_pok)
{
    id_pokazanie = id_pok;
    ui->lblCounter->setText(db.is_NameCounter(id_pok));
}

void NewCounter::sl_Ok()
{   
    db.UpdatePokazanieHome(id_pokazanie,ui->lEdCounter->text().toInt());
    close();
}
