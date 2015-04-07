#ifndef DIALOG_WITH_LINEEDIT_H
#define DIALOG_WITH_LINEEDIT_H

#include <QDialog>
#include <QAbstractButton>

//#include "bd.h"
#include "service.h"

namespace Ui {
class Dialog_with_LineEdit;
}

class Dialog_with_LineEdit : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_with_LineEdit(QWidget *parent = 0);
    ~Dialog_with_LineEdit();

private:
    Ui::Dialog_with_LineEdit *ui;
//    BD db;

signals:
    void sgn_string(QString str,int id_type_usluga);
private slots:
    void sl_Ok();
};

#endif // DIALOG_WITH_LINEEDIT_H
