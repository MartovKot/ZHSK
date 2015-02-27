#include "tableviewpokazanie.h"
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QModelIndex>
#include <QMessageBox>

TableViewPokazanie::TableViewPokazanie(QWidget *parent) :
    QTableView(parent)
{
}

void TableViewPokazanie::keyPressEvent(QKeyEvent *event)
{
    index = this->currentIndex();//сохраняем позицию перед редактированием
    int r = index.row()+1;
    QModelIndex index2;
    QAbstractItemView::keyPressEvent(event);
    switch (event->key()){
    case Qt::Key_Enter :
    case Qt::Key_Return:
        *event = QKeyEvent(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier);
        QAbstractItemView::keyPressEvent(event);       
        if(r<this->model()->rowCount()){
            index2 = this->model()->index(r,index.column());//смещаем на строку вниз
        }else{
            if (QMessageBox::question(this,trUtf8("Редактирование завершено"),
                                     trUtf8("Редактирование завершено.\n Выйти?"),
                                      QMessageBox::Ok,QMessageBox::Cancel) == QMessageBox::Ok){
                emit closing();
            }
        }
        setCurrentIndex(index2);//выделяем
        break;

    default: break;
    }

    return;
}
