#include "selecter_with_combobox.h"
#include "ui_selecter_with_combobox.h"

Selecter_with_ComboBox::Selecter_with_ComboBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Selecter_with_ComboBox)
{
    ui->setupUi(this);
    this->adjustSize();
    setLayout(ui->verticalLayout);
    connect(ui->comboBox,SIGNAL(activated(QString)),SIGNAL(CurrentValue(QString)));
}

Selecter_with_ComboBox::~Selecter_with_ComboBox()
{
    delete ui;
}

void Selecter_with_ComboBox::setContentsComboBox(QStringList strL)
{
    ui->comboBox->setItemText(0,"");
    for (int i=1;i<=strL.count();i++){
        ui->comboBox->setItemText(i,strL.at(i));
    }
}

void Selecter_with_ComboBox::setContentsComboBox(QSqlQueryModel* model)
{
    ui->comboBox->clear();
    ui->comboBox->addItem(trUtf8("Сделайте выбор"));

    for (int i=0;i<model->rowCount();i++){
        ui->comboBox->addItem(model->index(i,0).data().toString());
    }
}
