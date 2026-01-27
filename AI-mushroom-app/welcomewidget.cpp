#include "welcomewidget.h"
#include <QVBoxLayout>
#include <QTimer>

WelcomeWidget::WelcomeWidget(QWidget *parent) : QWidget(parent) {
    setupUi();
    setupAnimation();
}

void WelcomeWidget::setupUi() {
    // Set fixed size to match MainWindow
    setFixedSize(1000, 600);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // Create placeholder logo label
    // TODO: Replace this placeholder text with actual logo image (QPixmap) when available
    logoLabel = new QLabel("LOGO", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet(
        "font-size: 72px; "
        "font-weight: bold; "
        "color: #3B7A57; "
    );

    // Add opacity effect
    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.0);
    logoLabel->setGraphicsEffect(opacityEffect);

    layout->addWidget(logoLabel);
}

void WelcomeWidget::setupAnimation() {
    // Create animation group for fade-in, hold, fade-out
    animationGroup = new QSequentialAnimationGroup(this);

    // Fade-in animation (0.5s)
    QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    // Hold animation (1s) - pause with full opacity
    QPropertyAnimation *hold = new QPropertyAnimation(opacityEffect, "opacity");
    hold->setDuration(1000);
    hold->setStartValue(1.0);
    hold->setEndValue(1.0);

    // Fade-out animation (0.5s)
    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(500);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    // Add animations to group
    animationGroup->addAnimation(fadeIn);
    animationGroup->addAnimation(hold);
    animationGroup->addAnimation(fadeOut);

    // Connect animation finished signal
    connect(animationGroup, &QSequentialAnimationGroup::finished, this, &WelcomeWidget::finished);

    // Start animation automatically
    animationGroup->start();
}
