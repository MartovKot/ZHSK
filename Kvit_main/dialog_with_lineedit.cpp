#include "dialog_with_lineedit.h"
#include "ui_dialog_with_lineedit.h"
#include <QDebug>

Dialog_with_LineEdit::Dialog_with_LineEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_with_LineEdit)
{
    ui->setupUi(this);
    setLayout(ui->verticalLayout);
    this->adjustSize();
    connect(ui->buttonBox,SIGNAL(accepted()),SLOT(sl_Ok()));

    ui->comboBox->setModel(Service::modelServiceType());
}

Dialog_with_LineEdit::~Dialog_with_LineEdit()
{
    delete ui;
}

void Dialog_with_LineEdit::sl_Ok()
{
    sgn_string(ui->lineEdit->text(),
               ui->comboBox->model()->index(ui->comboBox->currentIndex(),1).data().toInt());
}
