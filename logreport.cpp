#include "logreport.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

LogReport::LogReport(QObject *parent) :
    QObject(parent)
{
}
void LogReport::setFileName(QString filename)
{
    FileName = filename;
}

void LogReport::logout(QString log)
{
    QFile *file = new QFile;
    file->setFileName(FileName);
    if(file->open(QIODevice::ReadWrite| QIODevice::Text)){
        file->seek(file->size());
        QTextStream out(file);
        out << QDateTime::currentDateTime().toString("yyMMdd - hh:mm:ss:zzz ") << log << "\n";
        file->close();
    }
    delete file;
}
