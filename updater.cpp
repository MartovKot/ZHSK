#include "updater.h"
#include <QTimer>

Updater::Updater(QWidget *parent) :
    QWidget(parent)
{

}

void Updater::RunUpdate()
{
    QTimer t;
    t.start(10000);
}
