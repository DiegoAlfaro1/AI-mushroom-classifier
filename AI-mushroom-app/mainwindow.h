#include "apiclient.h"
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

    // UI Pointers
    QPushButton *closeBtn;
    QPushButton *analyzeBtn;
    QString currentFilePath;
    QLabel *imageLabel;
    QLabel *resultTitle;
    QLabel *resultSubtitle;
    QProgressBar *confidenceBar;
    QLabel *safetyBanner;
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    struct PredictionRow {
        QLabel* nameLabel;
        QProgressBar* bar;
    };

    QList<PredictionRow> secondaryPredictions;

    ApiClient *apiClient;
    const QString API_URL = "http://localhost:5000/predict";

private slots:
    void resetImage();
    void onAnalyzeClicked();
    void onAnalysisSuccess(QList<Prediction> results);
    void onAnalysisError(QString message);
};
