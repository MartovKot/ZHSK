#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H
#include <QThread>
#include <QProgressDialog>
#include <QWidget>

class ProgressDialog : public QThread
{
    Q_OBJECT

public:
    ProgressDialog(const QString &labelText, const QString &cancelButtonText,
                   int minimum, int maximum, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ProgressDialog();
    void run();
    void setValue(int progress);
    QProgressDialog progress;

};

#endif // PROGRESSDIALOG_H
