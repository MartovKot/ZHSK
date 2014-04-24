#ifndef SELECTER_WITH_COMBOBOX_H
#define SELECTER_WITH_COMBOBOX_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QDebug>

namespace Ui {
class Selecter_with_ComboBox;
}

class Selecter_with_ComboBox : public QDialog
{
    Q_OBJECT

public:
    explicit Selecter_with_ComboBox(QWidget *parent = 0);
    ~Selecter_with_ComboBox();
    void setContentsComboBox(QStringList strL);
    void setContentsComboBox(QSqlQueryModel *model);

private:
    Ui::Selecter_with_ComboBox *ui;
signals:
    void CurrentValue(QString str);
};

#endif // SELECTER_WITH_COMBOBOX_H
