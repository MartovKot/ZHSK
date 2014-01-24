#include "updater.h"
#include <QTimer>

Updater::Updater(QWidget *parent) :
    QWidget(parent)
{

}

void Updater::RunUpdate()
{
    QNetworkRequest m_request;
    QNetworkAccessManager* m_manager;

    QString m_url = "https://api.github.com/repos/MartovKot/ZHSK/releases";

    //настройки прокси вынести отдельно
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("10.62.0.9");
    proxy.setPort(3128);
    QNetworkProxy::setApplicationProxy(proxy);
    //

    m_manager = new QNetworkAccessManager(this);
    m_manager->setProxy(proxy);

    m_request.setUrl(m_url);
    m_reply = m_manager->get(m_request);

    QObject::connect(m_manager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(finishedSlot(QNetworkReply*)));  //После запроса получаем json ответ который разбираем

}

void Updater::finishedSlot(QNetworkReply*)
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
//    downloadButton->setEnabled(false);

    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void Updater::startRequest(QUrl url)
{

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void Updater::httpFinished()
{
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        progressDialog->hide();
        return;
    }

    progressDialog->hide();
    file->flush();
    file->close();


    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
//        downloadButton->setEnabled(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
//        if (QMessageBox::question(this, tr("HTTP"),
//                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
//                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
//        }
    } else {
        QString fileName = QFileInfo(url.path()).fileName();
//        statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
//        downloadButton->setEnabled(true);
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}
