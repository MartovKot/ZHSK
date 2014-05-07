#include "dateofunixformat.h"
#include <QDebug>

DateOfUnixFormat::DateOfUnixFormat(int year, int month, int day)
{
    setDate(year,month,day);
}

DateOfUnixFormat::DateOfUnixFormat(QDate date)
{
    setDate(date.year(),date.month(),date.day());
}

qint64 DateOfUnixFormat::Second(int delta_month /*= 0*/)
{
    QDate date;
    date = this->addMonths(delta_month);
    qint64 timeInUnix;
    const qint64 MS_COEF = 1000;
    QDateTime datetime;
    datetime.setTimeSpec(Qt::OffsetFromUTC);
    datetime.setDate(date);
    datetime.setDate(*this);
    timeInUnix = datetime.toMSecsSinceEpoch() / MS_COEF;
    return timeInUnix;
}
