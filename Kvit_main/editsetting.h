#ifndef EDITSETTING_H
#define EDITSETTING_H

#include <QDialog>
#include "bd.h"

namespace Ui {
class EditSetting;
}

class EditSetting : public QDialog
{
    Q_OBJECT

public:
    explicit EditSetting(QWidget *parent = 0);
    ~EditSetting();
    void setNameSetting(QString NameSetting);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditSetting *ui;
    QString  name_setting;
    BD db;
signals:
    void s_Ok();
};

#endif // EDITSETTING_H
