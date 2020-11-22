#include <QFrame>

#pragma once

class ControlBar : public QFrame {
    Q_OBJECT

public:
    explicit ControlBar(QWidget *parent);

signals:
    void pressed();
    void moved();
    void released();

private:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};
