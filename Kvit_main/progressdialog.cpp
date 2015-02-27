#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString &labelText, const QString &cancelButtonText,
                               int minimum, int maximum, QWidget *parent, Qt::WindowFlags flags)
{
    QProgressDialog progress(labelText, cancelButtonText, minimum, maximum, parent,flags);

    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Расчёт"));
}

ProgressDialog::~ProgressDialog()
{
    progress.setValue(progress.maximum());
}

void ProgressDialog::run()
{
    progress.show();
}

void ProgressDialog::setValue(int val_progress)
{
    progress.setValue(val_progress);
}


