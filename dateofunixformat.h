#ifndef DATEOFUNIXFORMAT_H
#define DATEOFUNIXFORMAT_H

#include <QObject>
#include <QDate>

class DateOfUnixFormat : public QDate
{
public:

    DateOfUnixFormat(int year, int month, int day);
    DateOfUnixFormat(QDate date);
    qint64 Second(int delta_month = 0);
//    qint64 Second_next_month();
//    qint64 Second_previous_month();
//private:
//    qint64 second
};

#endif // DATEOFUNIXFORMAT_H
