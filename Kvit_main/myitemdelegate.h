#ifndef MYITEMDELEGATE_H
#define MYITEMDELEGATE_H
#include<QItemDelegate>

class MyItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit MyItemDelegate(QObject *parent=0);
    ~MyItemDelegate ();

    QWidget * createEditor( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};
#endif // MYITEMDELEGATE_H
