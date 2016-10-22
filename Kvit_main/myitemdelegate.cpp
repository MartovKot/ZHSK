#include "myitemdelegate.h"
#include <QLineEdit>
#include <QRegExp>
#include <QDebug>

MyItemDelegate::MyItemDelegate(QObject *parent)
    : QItemDelegate (parent)
{
}
MyItemDelegate::~MyItemDelegate()
{

}

QWidget * MyItemDelegate :: createEditor( QWidget * parent,
                                          const QStyleOptionViewItem & option,
                                          const QModelIndex & index ) const
{
    Q_UNUSED (option);

    QRegExpValidator *only_number;
    only_number = new QRegExpValidator(QRegExp("[0-9]+"),parent);   //валидатор для цифр

    QRegExpValidator *dv;
    dv = new QRegExpValidator(QRegExp("[0-9]+[\\.|\\,]?[0-9]{,2}"),parent);       //валидатор для чисел

    QLineEdit *led = 0;
    led = new QLineEdit(parent);
    if (index.row() == 8){
        led->setValidator(only_number);
    } else if (index.row() != 1 && index.row() != 2 && index.row() != 3){
        led->setValidator(dv);
    }

    return led;
}

void MyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lEd_1 = static_cast<QLineEdit*>(editor);
    model->setData(index, lEd_1->text(), Qt::EditRole);
}
