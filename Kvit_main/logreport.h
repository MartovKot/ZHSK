#ifndef LOGREPORT_H
#define LOGREPORT_H

#include <QObject>

class LogReport : public QObject
{
    Q_OBJECT
public:
    explicit LogReport(QObject *parent = 0);
    void logout(QString log);
    void setFileName(QString filename);
private:
    QString FileName;
    
signals:
    
public slots:
    
};

#endif // LOGREPORT_H
