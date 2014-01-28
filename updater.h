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
#include <QTimer>

class Updater : public QWidget
{
    Q_OBJECT
public:
    explicit Updater(QWidget *parent = 0);
    ~Updater();
    void RunUpdate();
private:
    QNetworkProxy proxy;
    QUrl url;
    QNetworkReply *m_reply;
    QNetworkReply *reply_download;
    QNetworkAccessManager m_manager_download;
    QNetworkAccessManager *m_manager_json;
    QFile *file;
    QProgressDialog *progressDialog;
    bool httpRequestAborted;


    void downloadFile(QUrl url);
    void startRequest(QUrl url);

signals:

public slots:
private slots:
    void finished_json (QNetworkReply*);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

};

#endif // UPDATER_H
