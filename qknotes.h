#ifndef QKNOTES_H
#define QKNOTES_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "contentmanager.h"

namespace Ui {
class QkNotes;
}

class QkNotes : public QWidget
{
    Q_OBJECT

public:
    explicit QkNotes(QWidget *parent = nullptr);
    ~QkNotes() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_textEdit_textChanged();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::QkNotes *ui;
    ContentManager m_mgr;
    int m_changeCount;
    QSystemTrayIcon* m_trayIcon;

    void _initTrayIcon();
    void _saveContent();
    void _setPosition();


    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // QKNOTES_H
