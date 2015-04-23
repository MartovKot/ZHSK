#include "editsetting.h"
#include "ui_editsetting.h"
#include <QDebug>

EditSetting::EditSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSetting)
{
    ui->setupUi(this);
    setLayout(ui->mainLay);
    this->adjustSize();
    ui->lEd_NameSetting->setText(name_setting);
}

EditSetting::~EditSetting()
{
    delete ui;
}

void EditSetting::setNameSetting(QString NameSetting)
{
    name_setting = NameSetting;
    ui->lEd_NameSetting->setText(name_setting);
    ui->lEd_ValueSetting->setText(Settings::Value(NameSetting));
}

void EditSetting::on_buttonBox_accepted()
{
    QString name;
    QString value;

    name = ui->lEd_NameSetting->text();
    value = ui->lEd_ValueSetting->text();
//    Settings::

    //EDIT
    BD::UpdateTable("settings","value_setting", value,"name_setting",name_setting);
    BD::UpdateTable("settings","name_setting", name,"name_setting",name_setting);

    emit s_Ok();
}
