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

private slots:
    void resetImage();
};
