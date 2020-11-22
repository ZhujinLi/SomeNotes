#include "controlbar.h"
#include "common.h"

ControlBar::ControlBar(QWidget *parent) : QFrame(parent) {
    setFrameShape(QFrame::HLine);
    setMinimumSize(QSize(50, 5));
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    setFrameShadow(QFrame::Shadow::Sunken);
    setLineWidth(1);
    setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
}

void ControlBar::mousePressEvent(QMouseEvent *event) {
    emit(pressed());
    QFrame::mousePressEvent(event);
}

void ControlBar::mouseReleaseEvent(QMouseEvent *event) {
    emit(released());
    QFrame::mouseReleaseEvent(event);
}

void ControlBar::mouseMoveEvent(QMouseEvent *event) {
    emit(moved());
    QFrame::mouseMoveEvent(event);
}
