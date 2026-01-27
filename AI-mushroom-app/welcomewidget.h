#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

class WelcomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = nullptr);

signals:
    void finished();

private:
    QLabel *logoLabel;
    QGraphicsOpacityEffect *opacityEffect;
    QSequentialAnimationGroup *animationGroup;

    void setupUi();
    void setupAnimation();
};

#endif // WELCOMEWIDGET_H
