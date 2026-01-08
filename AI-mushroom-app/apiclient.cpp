#include "apiclient.h"
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QMimeDatabase>

ApiClient::ApiClient(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

}

void ApiClient::analyzeImage(const QString &filePath, const QString &apiUrl) {
    qDebug() << "Preparing to upload:" << filePath << "to" << apiUrl;

    // 1. Prepare File
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit analysisFailed("Could not open image file.");
        delete file;
        return;
    }

    // 2. Setup Multipart
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QString("form-data; name=\"file\"; filename=\"%1\"").arg(QFileInfo(filePath).fileName()));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    // 3. Send Request
    QNetworkRequest request(apiUrl);
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    // --- CRITICAL CHANGE: Debugging & Error Handling ---

    // A. Connect specific error signal (Works best in Qt 6)
    connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError code){
        qDebug() << "❌ CRITICAL NETWORK ERROR:" << code << reply->errorString();
    });

    // B. Connect Upload Progress (Optional: to see if it's stuck uploading)
    connect(reply, &QNetworkReply::uploadProgress, this, [](qint64 bytesSent, qint64 bytesTotal){
        qDebug() << "Uploading:" << bytesSent << "/" << bytesTotal;
    });

    // Note: We don't need to manually connect 'finished' here because
    // the manager's global 'finished' signal (connected in constructor) will still fire.
}

void ApiClient::onNetworkReply(QNetworkReply *reply) {

    qDebug() << "✅ Request Finished. URL:" << reply->url().toString();
    qDebug() << "   Error Code:" << reply->error();
    qDebug() << "   HTTP Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // Check for network errors
    if (reply->error() != QNetworkReply::NoError) {
        QString err = reply->errorString();
        qDebug() << "   -> EMITTING FAILURE:" << err;
        emit analysisFailed("Network Error: " + err);
        reply->deleteLater();
        return;
    }

    // Parse JSON Response
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    // EXPECTED JSON FORMAT:
    // {
    //   "predictions": [
    //      {"species": "Amanita muscaria", "confidence": 0.98},
    //      {"species": "Boletus edulis", "confidence": 0.01}
    //   ]
    // }

    if (!root.contains("precitions")){
        emit analysisFailed("Invalid API Response: Missing 'predictions' key");
        reply->deleteLater();
        return;
    }


    QList<Prediction> results;
    QJsonArray array = root["predictions"].toArray();

    for (const QJsonValue &val : array) {
        QJsonObject obj = val.toObject();
        Prediction p;
        p.species = obj["species"].toString();
        // Handle both 0-1 and 0-100 scales gracefully
        double rawConf = obj["confidence"].toDouble();
        p.confidence = (rawConf > 1.0) ? rawConf / 100.0 : rawConf;
        results.append(p);
    }

    emit analysisFinished(results);
    reply->deleteLater();
}
