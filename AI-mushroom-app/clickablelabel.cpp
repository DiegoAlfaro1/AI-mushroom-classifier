#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f) {
    setCursor(Qt::PointingHandCursor);
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f) {
    setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QLabel::mousePressEvent(event);
}
