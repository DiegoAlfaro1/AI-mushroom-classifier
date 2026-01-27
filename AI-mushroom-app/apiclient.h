#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct Prediction {
    QString species;
    float confidence;
};

class ApiClient : public QObject{

    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    // Main function to call from Ui
    void analyzeImage(const QString &filePath, const QString &url);

    // Health check function
    void checkHealth(const QString &healthUrl);

signals:

    // Signal emitted when data is ready (UI connects to this)
    void analysisFinished(QList<Prediction> results);

    // Signal emitted on errror (network or server error)
    void analysisFailed(QString errorMsg);

    // Signal emitted when health check completes
    void healthCheckFinished(bool isHealthy, QString message);

private slots:

    void onNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;

};

#endif // APICLIENT_H
