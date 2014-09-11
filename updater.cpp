#include "updater.h"
#include <QProcess>

Updater::Updater(QWidget *parent) :
    QWidget(parent)
{
    //настройки прокси вынести отдельно
    if (db.isValueSetting("proxy_HostName") != "" || db.isValueSetting("proxy_Port") != ""){
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(db.isValueSetting("proxy_HostName"));
        proxy.setPort(db.isValueSetting("proxy_Port").toInt());
        m_manager_download.setProxy(proxy);
    }
    //


#if defined(QT_NO_SSL)
    connect(&m_manager_download, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif


}
Updater::~Updater()
{
    delete m_manager_json;
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
            if (QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate)
                                                                                        .toMSecsSinceEpoch() > last_time ){
                IndexLastVersion = i;  //номер блока где есть последняя версия
                last_time = QDateTime::fromString(m_jdoc.array().takeAt(i).toObject().value("published_at").toString(),Qt::ISODate)
                        .toMSecsSinceEpoch();
            }
        }

        if(isNewVersion(version, m_jdoc.array().takeAt(IndexLastVersion).toObject().value("tag_name").toString()) && IndexLastVersion != -1){
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
//        QMessageBox::critical(this, trUtf8("Ошибка"),
//                                          tr("Ошибка соединения %1").arg(m_reply->errorString()),
//                                          QMessageBox::Ok);

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
    m_url = url;
    startRequest(m_url);

}

void Updater::startRequest(QUrl url)
{
//    QFileInfo fileInfo(m_url.path());
//    QString fileName = fileInfo.fileName();

    progressDialog->open();
    reply_download = m_manager_download.get(QNetworkRequest(url));

    connect(reply_download, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply_download, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply_download, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void Updater::httpFinished()  //Завершение загрузки
{
    QUrl url;

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
        QFileInfo fileInfo(m_url.path());
        QString fileName = fileInfo.fileName();
        if (QMessageBox::question(this, trUtf8("Обновление"),
                                          tr("Обновление готово к установке. \n Обновить?"),
                                          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

            QProcess *upd = new QProcess;
            upd->start(fileName);
//            connect(vec,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finish_script()));
            emit s_run_update();
        }

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
    if (file)
        file->write(reply_download->readAll());
}

void Updater::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}

#ifndef QT_NO_SSL
void Updater::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
//#ifdef HAVE_QT
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
//#else
//    Q_UNUSED(errors)
//#endif
}
#endif


void Updater::setVersion(QString ver)
{
    version = ver;
}


bool Updater::isNewVersion(QString realVersion, QString downloadVersion)
{
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

    if(downloadVersionArr.at(0) == realVersionArr.at(0)){
        if(downloadVersionArr.at(1) == realVersionArr.at(1)){
            if(downloadVersionArr.at(2) <= realVersionArr.at(2)){
                return false;
            }
        }else if (downloadVersionArr.at(1) < realVersionArr.at(1)){
            return false;
        }
    }else if (downloadVersionArr.at(0) < realVersionArr.at(0)){
        return false;
    }
    //иначе есть новая версия
    return true;
}
