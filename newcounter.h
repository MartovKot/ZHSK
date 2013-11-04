#ifndef NEWCOUNTER_H
#define NEWCOUNTER_H

#include <QDialog>
#include "bd.h"

namespace Ui {
class NewCounter;
}

class NewCounter : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewCounter(QWidget *parent = 0);
    ~NewCounter();
    void set_IdPokazanie(int id_pokazanie);
    
private:
    Ui::NewCounter *ui;
    int id_pokazanie;
    BD db;
private slots:
    void sl_Ok();
};

#endif // NEWCOUNTER_H
