#include "updater.h"
//#include "httpwindow.h"

Updater::Updater(QWidget *parent) :
    QWidget(parent)
{
    //настройки прокси вынести отдельно
//    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("10.62.0.9");
    proxy.setPort(3128);
    QNetworkProxy::setApplicationProxy(proxy);
    //
//    m_manager_download = new QNetworkAccessManager(this);
    m_manager_download.setProxy(proxy);
#ifndef QT_NO_SSL
    connect(&m_manager_download, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif


}
Updater::~Updater()
{

}

void Updater::RunUpdate()
{
    QString m_url = "https://api.github.com/repos/MartovKot/ZHSK/releases";

    progressDialog = new QProgressDialog(this);
    m_manager_json = new QNetworkAccessManager(this);

    m_manager_json->setProxy(proxy);
    m_reply = m_manager_json->get(QNetworkRequest(m_url));

    QObject::connect(m_manager_json, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(finished_json(QNetworkReply*)));  //После запроса получаем json ответ который разбираем
}

void Updater::finished_json(QNetworkReply*)
{
//    QVariant statusCodeV = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (m_reply->error() == QNetworkReply::NoError)
    {
        QString json_reply(m_reply->readAll()); // json ответ от сервера

        QJsonDocument m_jdoc;
        m_jdoc = QJsonDocument::fromJson(json_reply.toUtf8());

        qint64 last_time = 0;
        QString path = "";
        QStringList path_list;

        for (int i = 0;i < m_jdoc.array().size();i++){
            if (QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch() > last_time){
                last_time = QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch();
                path += "https://github.com/MartovKot/ZHSK/releases";
                path += "/download/";
                path += m_jdoc.array().takeAt(i).toObject().value("tag_name").toString();
                path += "/";
                for (int j = 0; j < m_jdoc.array().takeAt(i).toObject().value("assets").toArray().size(); j++){
                    path_list << path + m_jdoc.array().takeAt(i).toObject().value("assets").toArray().takeAt(i).toObject().value("name").toString();
                }
            }
        }
        for (int i=0;i<path_list.size();i++){
            downloadFile(QUrl(path_list.at(i)));
        }
//        HttpWindow *HttpWin;
//        HttpWin = new HttpWindow;
//        HttpWin->show();
//        HttpWin->setUrl(path_list.at(0));
    }
    // Some http error received
    else
    {
       qDebug()<< m_reply->errorString();
       // handle errors here
    }

    delete m_reply;
    delete m_manager_json;
}

void Updater::downloadFile(QUrl url)
{

    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.html";

    if (QFile::exists(fileName)) {
//        if (QMessageBox::question(this, tr("HTTP"),
//                                  tr("There already exists a file called %1 in "
//                                     "the current directory. Overwrite?").arg(fileName),
//                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
//            == QMessageBox::No)
//            return;
        QFile::remove(fileName);
        qDebug()<<"delete old file: " << fileName;
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"),
                                 trUtf8("Невозможно сохранить файл %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        return;
    }

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));

    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void Updater::startRequest(QUrl url)
{
    progressDialog->open();

//    qDebug() << "Start Request: " << url;

    qDebug() << "Start Request";
    reply_download = m_manager_download.get(QNetworkRequest(url));

    connect(reply_download, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply_download, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply_download, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void Updater::httpFinished()
{
    qDebug()<<"httpFinished";
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply_download->deleteLater();
        progressDialog->hide();
        return;
    }

    progressDialog->hide();
    file->flush();
    file->close();


    QVariant redirectionTarget = reply_download->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply_download->error()) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply_download->errorString()));
//        downloadButton->setEnabled(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply_download->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
        }
    } else {
        QString fileName = QFileInfo(url.path()).fileName();
//        statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
//        downloadButton->setEnabled(true);
    }

    reply_download->deleteLater();
    reply_download = 0;
    delete file;
    file = 0;
}

void Updater::httpReadyRead()
{
    qDebug()<<"httpReadyRead";
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply_download->readAll());
}

void Updater::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    qDebug()<<"updateDataReadProgress";
    if (httpRequestAborted)
        return;
    qDebug()<<totalBytes;
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}

#ifndef QT_NO_SSL
void Updater::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply_download->ignoreSslErrors();
    }
}
#endif
