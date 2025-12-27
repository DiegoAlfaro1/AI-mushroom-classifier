#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- PASTE THE STYLES HERE ---
    // We use a raw string literal R"(...)" so we can write CSS normally
    QString styleSheet = R"(
        /* GLOBAL SETTINGS */
        QWidget {
            background-color: #F9F7F2; /* Creamy off-white */
            font-family: "Segoe UI", "Roboto", sans-serif;
            color: #2D2D2D;
        }

        /* LEFT PANEL - DROP ZONE */
        QLabel#imageDropZone {
            border: 3px dashed #C0C0C0;
            border-radius: 12px;
            background-color: #ECEAE5;
            color: #7A7A7A;
            font-size: 18px;
            font-weight: bold;
        }

        /* RIGHT PANEL - SAFETY BANNER */
        QLabel#safetyBanner {
            background-color: #D97706; /* Warning Orange */
            color: white;
            padding: 10px;
            border-radius: 6px;
            font-weight: bold;
            font-size: 11px;
        }

        /* TYPOGRAPHY */
        QLabel#resultTitle {
            font-size: 32px;
            font-weight: 800;
            color: #1A1A1A;
        }

        QLabel#resultSubtitle {
            font-size: 18px;
            font-style: italic;
            color: #555555;
        }

        /* CONFIDENCE BAR */
        QProgressBar {
            border: none;
            background-color: #E0E0E0;
            color: #FFFFFF;
            border-radius: 6px;
            height: 30px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #2F855A; /* Forest Green */
            border-radius: 6px;
        }

        /* BUTTONS */
        QPushButton {
            background-color: #FFFFFF;
            border: 1px solid #D1D5DB;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #F3F4F6;
            border-color: #9CA3AF;
        }

        QPushButton#primaryBtn {
            background-color: #2F855A;
            color: white;
            border: none;
            padding: 12px;
        }
        QPushButton#primaryBtn:hover {
            background-color: #276749;
        }
        QLabel#secondaryName {
            font-size: 14px;
            color: #4A4A4A;
        }

        QLabel#secondaryScore {
            font-size: 13px;
            font-weight: bold;
            color: #000000;
        }

        /* SECONDARY PROGRESS BARS */
        QProgressBar#secondaryBar {
            background-color: #E5E7EB; /* Light Grey background */
            border-radius: 3px;
            color: #000000;
            border: none;
        }
        QProgressBar#secondaryBar::chunk {
            background-color: #9CA3AF; /* Muted Grey-Blue for the fill */
            border-radius: 3px;
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

    MainWindow w;
    w.show();
    return a.exec();
}
