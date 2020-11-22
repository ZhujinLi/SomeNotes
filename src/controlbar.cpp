#include "controlbar.h"

ControlBar::ControlBar(QWidget *parent) : QFrame(parent) {
    setFrameShape(QFrame::HLine);
    setMinimumSize(QSize(50, 5));
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    setFrameShadow(QFrame::Shadow::Sunken);
    setLineWidth(1);
    setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
}
