#include "updater.h"
#include <QSplashScreen>

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
//    qDebug()<<"Start update";

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
        int IndexLastVersion = -1;
        QString path = "";
        QStringList path_list;

        for (int i = 0;i < m_jdoc.array().size();i++){  // проходим блоки
            if (QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch() > last_time ){
                IndexLastVersion = i;  //номер блока где есть последняя версия
                last_time = QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate).toMSecsSinceEpoch();
            }
        }

        if(isNewVersion(version, m_jdoc.array().takeAt(IndexLastVersion).toObject().value("tag_name").toString()) && IndexLastVersion != -1){
            qDebug() << "test";
            if (QMessageBox::question(this, trUtf8("Обновление"),
                                              tr("Найдено новое обновление "
                                                 "Скачать?"),
                                              QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                        == QMessageBox::No){
                       return;
            }
            path += "https://github.com/MartovKot/ZHSK/releases";
            path += "/download/";
            path += m_jdoc.array().takeAt(IndexLastVersion).toObject().value("tag_name").toString();
            path += "/";
            for (int j = 0; j < m_jdoc.array().takeAt(IndexLastVersion).toObject().value("assets").toArray().size(); j++){
                path_list << path + m_jdoc.array().takeAt(IndexLastVersion).toObject().value("assets").toArray().takeAt(IndexLastVersion).toObject().value("name").toString();
            }

        }
        for (int i=0;i<path_list.size();i++){
            downloadFile(QUrl(path_list.at(i)));
        }
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
//        qDebug()<<"delete old file: " << fileName;
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
//    qDebug()<<"httpFinished";
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
//        if (QMessageBox::question(this, tr("HTTP"),
//                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
//                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply_download->deleteLater();
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

    reply_download->deleteLater();
    reply_download = 0;
    delete file;
    file = 0;
}

void Updater::httpReadyRead()
{
//    qDebug()<<"httpReadyRead";
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply_download->readAll());
}

void Updater::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
//    qDebug()<<"updateDataReadProgress";
    if (httpRequestAborted)
        return;
//    qDebug()<<totalBytes;
//    progressDialog->show();
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

//    if (QMessageBox::warning(this, tr("HTTP"),
//                             tr("One or more SSL errors has occurred: %1").arg(errorString),
//                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply_download->ignoreSslErrors();
//    }
}
#endif


void Updater::setVersion(QString ver)
{
    version = ver;
}


bool Updater::isNewVersion(QString realVersion, QString downloadVersion)
{
//    qDebug() << realVersion << downloadVersion;
    QList<int> realVersionArr;
    QList<int> downloadVersionArr;
    int pos;
    QString t_rVer = realVersion;
    QString t_dVer = downloadVersion;

    pos = t_rVer.indexOf(".");
    while(pos != -1){
        realVersionArr << t_rVer.left(pos).toInt();
        t_rVer = t_rVer.mid(pos+1);
        pos = t_rVer.indexOf(".",pos);
    }
    realVersionArr << t_rVer.toInt();

    pos = t_dVer.indexOf(".");
    while(pos != -1){
        downloadVersionArr << t_dVer.left(pos).toInt();
        t_dVer = t_dVer.mid(pos+1);
        pos = t_dVer.indexOf(".",pos);
    }
    downloadVersionArr << t_dVer.toInt();

//    qDebug() << realVersionArr << downloadVersionArr;

//    qDebug()<<"test" <<realVersionArr.count();

    if(downloadVersionArr.at(0) == realVersionArr.at(0)){
        if(downloadVersionArr.at(1) == realVersionArr.at(1)){
            if(downloadVersionArr.at(2) <= realVersionArr.at(2)){
//                qDebug()<<"flase - 1";
                return false;
            }
        }else if (downloadVersionArr.at(1) < realVersionArr.at(1)){
//            qDebug()<<"flase - 2";
            return false;
        }
    }else if (downloadVersionArr.at(0) < realVersionArr.at(0)){

//        qDebug()<<"flase - 3" << downloadVersionArr.at(1) << realVersionArr.at(1);
        return false;
    }
//    qDebug()<<"true";
    //иначе есть новая версия
    return true;
}
