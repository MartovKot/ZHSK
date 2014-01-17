#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>

class Updater : public QWidget
{
    Q_OBJECT
public:
    explicit Updater(QWidget *parent = 0);
    void RunUpdate();

signals:

public slots:

};

#endif // UPDATER_H
