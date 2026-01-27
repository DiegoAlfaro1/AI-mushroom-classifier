#include "mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();

    // Enable Drop on the window
    setAcceptDrops(true);

    // --- API CLIENT SETUP ---
    apiClient = new ApiClient(this);
    connect(apiClient, &ApiClient::analysisFinished, this, &MainWindow::onAnalysisSuccess);
    connect(apiClient, &ApiClient::analysisFailed, this, &MainWindow::onAnalysisError);
    connect(apiClient, &ApiClient::healthCheckFinished, this, &MainWindow::onHealthCheckFinished);

    // Set Window Properties
    setWindowTitle("ShroomID - Deep Learning Classifier");
    resize(1000, 600);

    // Perform health check on startup
    apiClient->checkHealth(HEALTH_URL);
}

void MainWindow::setupUi() {
    // 1. Central Widget & Main Layout
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(30);

    QFrame *leftPanel = new QFrame();
    leftPanel->setFixedWidth(550);
    QVBoxLayout*leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(15);

    // --- LEFT PANEL (Image Display) ---
    imageLabel = new QLabel("Drop Image Here");
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->setObjectName("imageDropZone"); // For styling

    // --- Deleting image X button
    closeBtn = new QPushButton("✖", imageLabel);
    closeBtn->setFixedSize(30,30);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->hide();
    closeBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: rgba(0, 0, 0, 150);"
        "  color: #FFFFFF;"
        "  border-radius: 15px;" // Makes it a circle
        "  border: 1px solid white;"
        "  padding: 0px;"
        "  margin: 0px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #DC2626; }" // Red on hover
        );
    // Position it in the Top-Right corner of the imageLabel
    // We use a Layout inside the Label to stick it to the corner
    QVBoxLayout *labelLayout = new QVBoxLayout(imageLabel);
    labelLayout->setContentsMargins(10, 10, 10, 10); // Padding from edges
    labelLayout->addWidget(closeBtn, 0, Qt::AlignTop | Qt::AlignRight);

    // Connect the click signal to our reset function
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::resetImage);

    // --- Tittle ---
    titleLabel = new QLabel("SHROOMID");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");

    // --- Subtitle ---
    subtitleLabel = new QLabel("AI-Powered Mushroom Classification");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setObjectName("subtitleLabel");


    leftLayout->addWidget(titleLabel);
    leftLayout->addSpacing(4);
    leftLayout->addWidget(subtitleLabel);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(imageLabel);

    // --- RIGHT PANEL (Results & Controls) ---
    QFrame *rightPanel = new QFrame();
    rightPanel->setFixedWidth(450); // Fixed width for result sidebar
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);

    // A. Safety Banner
    safetyBanner = new QLabel("⚠️ AI Prediction Only. Never ingest based on this result.");
    safetyBanner->setWordWrap(true);
    safetyBanner->setObjectName("safetyBanner");

    // B. Results Text
    resultTitle = new QLabel("Results will appear here");
    resultTitle->setObjectName("resultTitle");

    // C. Confidence Bar
    QLabel *confLabel = new QLabel("Confidence Match:");
    confidenceBar = new QProgressBar();
    confidenceBar->setValue(100);
    confidenceBar->setTextVisible(true);
    confidenceBar->setFormat("%p%");

    // D. Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *wikiBtn = new QPushButton("View on Wikipedia ↗");
    QPushButton *simBtn = new QPushButton("Search Images ↗");
    btnLayout->addWidget(wikiBtn);
    btnLayout->addWidget(simBtn);

    // Add everything to Right Layout
    rightLayout->addWidget(safetyBanner);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(resultTitle);
    rightLayout->addSpacing(10);
    
    // Loading indicator
    loadingLabel = new QLabel("⏳ Analyzing...");
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2563EB; padding: 10px;");
    loadingLabel->hide(); // Hidden by default
    rightLayout->addWidget(loadingLabel);
    
    rightLayout->addWidget(confLabel);
    rightLayout->addWidget(confidenceBar);
    rightLayout->addLayout(btnLayout);
    rightLayout->addStretch(); // Pushes everything up

    analyzeBtn = new QPushButton("Analyze Image"); // Assign to class member
    analyzeBtn->setObjectName("primaryBtn");
    analyzeBtn->setCursor(Qt::PointingHandCursor);
    // Disable it initially until an image is dropped
    analyzeBtn->setEnabled(false);
    rightLayout->addWidget(analyzeBtn);

    // Connect the click signal
    connect(analyzeBtn, &QPushButton::clicked, this, &MainWindow::onAnalyzeClicked);

    // --- Secondary results section ---
    QLabel *secondaryHeader = new QLabel("Other possibilities:");
    secondaryHeader->setStyleSheet("font-weight: bold; font-size: 12px; color: #555; margin-top: 15px;");
    rightLayout->addWidget(secondaryHeader);

    for(int i = 0; i < 4; i++){
        QLabel *name = new QLabel("Alternative Species " + QString::number(i+1));
        QProgressBar *bar = new QProgressBar();
        name->setObjectName("secondaryName");
        bar->setObjectName("secondaryBar");
        bar->setStyleSheet("height: 15px");

        QHBoxLayout *textLayout = new QHBoxLayout();
        textLayout->addWidget(name);
        textLayout->addStretch(); // Push score to the right

        rightLayout->addLayout(textLayout);
        rightLayout->addWidget(bar);
        rightLayout->addSpacing(2); // Small gap between rows

        PredictionRow row;
        row.nameLabel = name;
        row.bar = bar;
        secondaryPredictions.append(row);
    }

    // --- FINAL ASSEMBLY ---
    mainLayout->addWidget(leftPanel, 2); // 66% width
    mainLayout->addWidget(rightPanel, 1); // 33% width
}

// Basic Drag & Drop Logic
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QString filePath = urls.first().toLocalFile();

    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix();

    qDebug() << "Extension: " << extension;

    // VALIDATION CHECK
    if(extension.toLower() != "png" && extension.toLower() != "jpg" && extension.toLower() != "jpeg" && extension.toLower() != "webp"){
        qDebug() << "ERROR: Unsupported file format";

        imageLabel -> setText("Unsupported format\n(JPG, PNG, or WEBP only)");
        imageLabel -> setStyleSheet("border: 2px dashed #DC2626; color: #DC2626; font-size: 18px; font-weight: bold; ");

        QTimer::singleShot(5000, this, [this](){
            imageLabel -> setText("Drop Image Here");

            imageLabel->setStyleSheet("border: 3px dashed #C0C0C0; border-radius: 12px; background-color: #ECEAE5; color: #7A7A7A; font-size: 18px; font-weight: bold;");
        });
        return;
    }

    qDebug() << "File with correct type";
    QPixmap pix(filePath);
    if (!pix.isNull()) {
        // Scale image to fit while keeping aspect ratio
        imageLabel->setPixmap(pix.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setText(""); // Remove "Drop Here" text
        closeBtn->show();
        closeBtn -> raise();

        // Store the image path for later use
        currentFilePath = filePath;

        // UI Feedback
        analyzeBtn->setEnabled(true);
        resultTitle->setText("Ready to Analyze");
        confidenceBar->setValue(0);
    }
}

// Reset image if not needed anymore
void MainWindow::resetImage(){

    imageLabel->setPixmap(QPixmap());
    imageLabel->setText("Drop Images Here");

    closeBtn->hide();

    imageLabel->setStyleSheet(R"(
        border: 3px dashed #C0C0C0;
        border-radius: 12px;
        background-color: #ECEAE5;
        color: #7A7A7A;
        font-size: 18px;
        font-weight: bold;
    )");

    qDebug() << "Image closed by user";
}

// Handle Success
void MainWindow::onAnalysisSuccess(QList<Prediction> results) {
    analyzeBtn->setEnabled(true);
    loadingLabel->hide(); // Hide loading indicator

    if (results.isEmpty()) return;

    Prediction top = results[0];
    resultTitle->setText(top.species);
    confidenceBar->setValue((int)(top.confidence * 100));

    // Fill secondary list (same logic as before)
    for (int i = 0; i < 4; i++) {
        if (i + 1 < results.size()) {
            secondaryPredictions[i].nameLabel->setText(results[i+1].species);
            secondaryPredictions[i].bar->setValue((int)(results[i+1].confidence * 100));
        } else {
            secondaryPredictions[i].nameLabel->setText("-");
            secondaryPredictions[i].bar->setValue(0);
        }
    }
}

// Handle Error
void MainWindow::onAnalysisError(QString message) {
    analyzeBtn->setEnabled(true);
    loadingLabel->hide(); // Hide loading indicator
    resultTitle->setText("Error");
    safetyBanner->setText("❌ " + message);
    safetyBanner->setStyleSheet("background-color: #DC2626; color: white; padding: 12px;");
}

void MainWindow::onAnalyzeClicked() {
    if (currentFilePath.isEmpty()) return;

    resultTitle->setText("Getting results...");
    loadingLabel->show(); // Show loading indicator
    analyzeBtn->setEnabled(false);

    apiClient->analyzeImage(currentFilePath, API_URL);
}

void MainWindow::onHealthCheckFinished(bool isHealthy, QString message) {
    qDebug() << "Health check completed:" << message;
    
    if (!isHealthy) {
        // Show warning in the subtitle if API is not healthy
        subtitleLabel->setText("⚠️ " + message);
        subtitleLabel->setStyleSheet("color: #DC2626; font-weight: bold;");
    } else {
        // API is healthy, show normal subtitle
        subtitleLabel->setText("AI-Powered Mushroom Classification");
        subtitleLabel->setStyleSheet("");
    }
}
