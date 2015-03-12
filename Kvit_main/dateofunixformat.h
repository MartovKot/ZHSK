#ifndef DATEOFUNIXFORMAT_H
#define DATEOFUNIXFORMAT_H

#include <QObject>
#include <QDate>

class DateOfUnixFormat : public QDate
{
public:
    DateOfUnixFormat();
    DateOfUnixFormat(int year, int month, int day);
    DateOfUnixFormat(QDate date);
    qint64 Second(int delta_month = 0);
    qint64 Second_first_day(int delta_month = 0);
};

#endif // DATEOFUNIXFORMAT_H
