#include "mainwindow.h"
#include "welcomewidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application icon from embedded SVG resource
    a.setWindowIcon(QIcon(":/assets/ShroomID-logo.svg"));

    // --- PASTE THE STYLES HERE ---
    // We use a raw string literal R"(...)" so we can write CSS normally
    QString styleSheet = R"(
        /* GLOBAL SETTINGS */
        QWidget {
            background-color: #ebe6da; /* Warm parchment */
            font-family: "Segoe UI", "Roboto", sans-serif;
            color: #2E2A24; /* Soft soil brown */
        }

        /* LEFT PANEL - DROP ZONE */
        QLabel#imageDropZone {
            border: 3px dashed #B8B0A2; /* Clay gray */
            border-radius: 14px;
            background-color: #EFECE4; /* Light mushroom cap */
            color: #6B665C;
            font-size: 18px;
            font-weight: 600;
        }

        /* TITLE */
        QLabel#titleLabel {
            font-size: 64px;
            font-weight: 900;
            color: #3B7A57; /* Deep forest green */
        }

        /* SUBTITLE */
        QLabel#subtitleLabel {
            font-size: 20px;
            font-weight: 400;
            color: #5F6F64; /* Mossy green-gray */
            letter-spacing: 0.8px;
        }

        /* RIGHT PANEL - SAFETY BANNER */
        QLabel#safetyBanner {
            background-color: #C97C2D; /* Burnt amber */
            color: #FFFFFF;
            padding: 10px;
            border-radius: 8px;
            font-weight: 700;
            font-size: 11px;
        }

        /* TYPOGRAPHY */
        QLabel#resultTitle {
            font-size: 32px;
            font-weight: 800;
            color: #2A2A2A;
        }

        QLabel#resultSubtitle {
            font-size: 18px;
            font-style: italic;
            color: #6B6B6B;
        }

        /* CONFIDENCE BAR */
        QProgressBar {
            border: none;
            background-color: #DDD8CF; /* Soft stone */
            color: #FFFFFF;
            border-radius: 8px;
            height: 30px;
            text-align: center;
        }

        QProgressBar::chunk {
            background-color: #4F8F68; /* Moss green */
            border-radius: 8px;
        }

        /* BUTTONS */
        QPushButton {
            background-color: #FFFFFF;
            border: 1px solid #CFC7BA;
            border-radius: 8px;
            padding: 10px 18px;
            font-weight: 600;
            color: #2E2A24;
        }

        QPushButton:hover {
            background-color: #F0EEE8;
            border-color: #A89F91;
        }

        /* PRIMARY ACTION */
        QPushButton#primaryBtn {
            background-color: #4F8F68; /* Forest moss */
            color: #FFFFFF;
            border: none;
            padding: 14px;
            font-weight: 700;
        }

        QPushButton#primaryBtn:hover {
            background-color: #3E7355;
        }

        /* SECONDARY RESULTS */
        QLabel#secondaryName {
            font-size: 14px;
            color: #5A554B;
        }

        QLabel#secondaryScore {
            font-size: 13px;
            font-weight: 700;
            color: #2E2A24;
        }

        /* SECONDARY PROGRESS BARS */
        QProgressBar#secondaryBar {
            background-color: #E4E1D9;
            border-radius: 4px;
            color: #2E2A24;
            border: none;
        }

        QProgressBar#secondaryBar::chunk {
            background-color: #9B8F7A; /* Earth brown */
            border-radius: 4px;
        }

    )";

    // Apply the styles to the whole app
    a.setStyleSheet(styleSheet);
    // -----------------------------

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AI-mushroom-app_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow *mainWindow = new MainWindow();
    WelcomeWidget *welcomeWidget = new WelcomeWidget();
    
    // Connect welcome widget finished signal to show main window
    QObject::connect(welcomeWidget, &WelcomeWidget::finished, [welcomeWidget, mainWindow]() {
        welcomeWidget->hide();
        mainWindow->show();
        welcomeWidget->deleteLater();
    });
    
    welcomeWidget->show();
    return a.exec();
}
