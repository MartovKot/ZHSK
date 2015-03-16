#ifndef MASSOPERATIONS_H
#define MASSOPERATIONS_H

#include <QDialog>
#include "bd.h"

namespace Ui {
class MassOperations;
}

class MassOperations : public QDialog
{
    Q_OBJECT

public:
    explicit MassOperations(QWidget *parent = 0);
    ~MassOperations();
    void setHome(int id_home);
    void setOrganization(int id_org);

private slots:
    void on_pBtn_Add_clicked();
    void on_pBtn_Delete_clicked();

private:
    Ui::MassOperations *ui;
    BD db;
    int m_id_home;
    int m_id_org;

signals:
    void end();
};

#endif // MASSOPERATIONS_H
