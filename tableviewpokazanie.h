#ifndef TABLEVIEWPOKAZANIE_H
#define TABLEVIEWPOKAZANIE_H
#include <QTableView>

class TableViewPokazanie: public QTableView
{
    Q_OBJECT
public:
    TableViewPokazanie(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);
private:
        QModelIndex index;
signals:
        void closing();
};

#endif // TABLEVIEWPOKAZANIE_H
