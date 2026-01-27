#include "apiclient.h"
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QMimeDatabase>

ApiClient::ApiClient(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    
    // Connect the finished signal to our slot
    connect(manager, &QNetworkAccessManager::finished, this, &ApiClient::onNetworkReply);
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

    // Check file size (10MB limit)
    qint64 fileSize = file->size();
    const qint64 MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB
    if (fileSize > MAX_FILE_SIZE) {
        emit analysisFailed("File too large. Maximum size is 10MB.");
        file->close();
        delete file;
        return;
    }

    // Validate file extension
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    QStringList allowedExtensions = {"jpg", "jpeg", "png", "webp"};
    if (!allowedExtensions.contains(extension)) {
        emit analysisFailed("Unsupported file format. Allowed: JPG, PNG, WEBP.");
        file->close();
        delete file;
        return;
    }

    // 2. Setup Multipart
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    // Add image part
    QHttpPart imagePart;
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, mime.name());
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QString("form-data; name=\"file\"; filename=\"%1\"").arg(QFileInfo(filePath).fileName()));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    // Add top_k parameter
    QHttpPart topKPart;
    topKPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                      QVariant("form-data; name=\"top_k\""));
    topKPart.setBody("5"); // Hardcoded to 5 (1 top + 4 secondary predictions)
    multiPart->append(topKPart);

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

    // Get HTTP status code
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // Check for network errors with granular error messages
    if (reply->error() != QNetworkReply::NoError) {
        QString err;
        
        // Provide specific error messages based on status codes
        if (httpStatus == 413) {
            err = "File too large. The server rejected the upload (max 10MB).";
        } else if (httpStatus == 503) {
            err = "Service unavailable. The model may not be loaded yet.";
        } else if (httpStatus == 415) {
            err = "Unsupported media type. Please use JPG, PNG, or WEBP.";
        } else if (httpStatus == 422) {
            err = "Invalid request. Please check the image file.";
        } else if (httpStatus == 500) {
            err = "Server error. Please try again later.";
        } else if (httpStatus >= 400 && httpStatus < 500) {
            err = "Client error (" + QString::number(httpStatus) + "): " + reply->errorString();
        } else if (httpStatus >= 500) {
            err = "Server error (" + QString::number(httpStatus) + "): " + reply->errorString();
        } else {
            err = "Network error: " + reply->errorString();
        }
        
        qDebug() << "   -> EMITTING FAILURE:" << err;
        emit analysisFailed(err);
        reply->deleteLater();
        return;
    }

    // Parse JSON Response
    QByteArray data = reply->readAll();
    qDebug() << "Raw response:" << data;
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        emit analysisFailed("Invalid JSON response from server.");
        reply->deleteLater();
        return;
    }
    
    QJsonObject root = doc.object();

    // EXPECTED JSON FORMAT (from API):
    // {
    //   "success": true,
    //   "message": "Prediction successful",
    //   "data": {
    //     "top_prediction": "Amanita muscaria",
    //     "confidence": 0.95,
    //     "top_k_predictions": [
    //        {"species": "Amanita muscaria", "confidence": 0.95},
    //        {"species": "Boletus edulis", "confidence": 0.01}
    //     ],
    //     "timestamp": "2024-01-13T12:00:00"
    //   }
    // }

    // Check for success field
    if (!root.contains("success") || !root["success"].toBool()) {
        QString message = root.value("message").toString("Unknown error");
        emit analysisFailed("API Error: " + message);
        reply->deleteLater();
        return;
    }

    // Navigate to data object
    if (!root.contains("data")) {
        emit analysisFailed("Invalid API Response: Missing 'data' field");
        reply->deleteLater();
        return;
    }

    QJsonObject dataObj = root["data"].toObject();

    // Check for top_k_predictions array
    if (!dataObj.contains("top_k_predictions")) {
        emit analysisFailed("Invalid API Response: Missing 'top_k_predictions' field");
        reply->deleteLater();
        return;
    }

    QList<Prediction> results;
    QJsonArray array = dataObj["top_k_predictions"].toArray();

    for (const QJsonValue &val : array) {
        QJsonObject obj = val.toObject();
        Prediction p;
        p.species = obj["species"].toString();
        // Handle both 0-1 and 0-100 scales gracefully
        double rawConf = obj["confidence"].toDouble();
        p.confidence = (rawConf > 1.0) ? rawConf / 100.0 : rawConf;
        results.append(p);
    }

    if (results.isEmpty()) {
        emit analysisFailed("No predictions returned from API.");
        reply->deleteLater();
        return;
    }

    emit analysisFinished(results);
    reply->deleteLater();
}

void ApiClient::checkHealth(const QString &healthUrl) {
    qDebug() << "Checking API health at:" << healthUrl;
    
    QNetworkRequest request(healthUrl);
    QNetworkReply *reply = manager->get(request);
    
    // Connect a lambda to handle health check response separately
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        bool isHealthy = false;
        QString message;
        
        if (reply->error() != QNetworkReply::NoError) {
            message = "API not reachable: " + reply->errorString();
        } else {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (!doc.isNull()) {
                QJsonObject root = doc.object();
                
                if (root.contains("success") && root["success"].toBool()) {
                    isHealthy = true;
                    
                    // Extract model status if available
                    if (root.contains("data")) {
                        QJsonObject dataObj = root["data"].toObject();
                        bool modelLoaded = dataObj.value("model_loaded").toBool();
                        QString status = dataObj.value("status").toString();
                        
                        if (modelLoaded) {
                            message = "API is healthy and model is loaded.";
                        } else {
                            message = "API is reachable but model is not loaded.";
                            isHealthy = false;
                        }
                    } else {
                        message = "API is healthy.";
                    }
                } else {
                    message = "API returned unsuccessful status.";
                }
            } else {
                message = "Invalid response from health endpoint.";
            }
        }
        
        qDebug() << "Health check result:" << (isHealthy ? "✅ Healthy" : "❌ Unhealthy") << "-" << message;
        emit healthCheckFinished(isHealthy, message);
        reply->deleteLater();
    });
}
