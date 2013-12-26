#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H
#include <QObject>

qint64 IsDateOfUnix(int year, int month, int day)
{
    qint64 timeInUnix;
    const qint64 MS_COEF = 1000;
    QDate date;
    date.setDate(year, month, day);
    QDateTime datetime;
    datetime.setTimeSpec(Qt::OffsetFromUTC);
    datetime.setDate(date);
    timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;
    return timeInUnix;
}

#endif // MY_FUNCTION_H
