#include "myitemdelegate.h"
#include <QLineEdit>
#include <QRegExp>

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

    QLineEdit *led = 0;
    if (index.column() == 2){
        led = new QLineEdit(parent);
        led->setValidator(only_number);
    }

    return led;
}

void MyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lEd_1 = static_cast<QLineEdit*>(editor);
    int value = lEd_1->text().toInt();

    model->setData(index, value, Qt::EditRole);
}
