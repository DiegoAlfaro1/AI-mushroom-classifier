#include "apiclient.h"
#include "clickablelabel.h"
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);


protected:
    // Basic Drag & Drop overrides
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setupUi();
    void loadAndDisplayImage(const QString& filePath);

    // UI Pointers
    QPushButton *closeBtn;
    QPushButton *analyzeBtn;
    QString currentFilePath;
    ClickableLabel *imageLabel;
    QLabel *resultTitle;
    QLabel *resultSubtitle;
    QProgressBar *confidenceBar;
    QLabel *safetyBanner;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *loadingLabel; // Loading spinner/indicator

    struct PredictionRow {
        QLabel* nameLabel;
        QProgressBar* bar;
    };

    QList<PredictionRow> secondaryPredictions;

    ApiClient *apiClient;
    const QString API_URL = "http://localhost:8000/api/v1/predict/";
    const QString HEALTH_URL = "http://localhost:8000/api/v1/health/";

private slots:
    void resetImage();
    void onAnalyzeClicked();
    void onImageZoneClicked();
    void onAnalysisSuccess(QList<Prediction> results);
    void onAnalysisError(QString message);
    void onHealthCheckFinished(bool isHealthy, QString message);
};
