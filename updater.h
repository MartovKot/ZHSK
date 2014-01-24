#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QUrl>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>

class Updater : public QWidget
{
    Q_OBJECT
public:
    explicit Updater(QWidget *parent = 0);
    void RunUpdate();
private:
    QUrl url;
    QNetworkReply *m_reply;
    QNetworkReply *reply;
    QNetworkAccessManager qnam;
    QFile *file;
    QProgressDialog *progressDialog;
    bool httpRequestAborted;


    void downloadFile(QUrl url);
    void startRequest(QUrl url);

signals:

public slots:
private slots:
    void finishedSlot (QNetworkReply*);
    void httpFinished();

};

#endif // UPDATER_H
