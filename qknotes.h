#ifndef QKNOTES_H
#define QKNOTES_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "contentmanager.h"
#include "noteblock.h"

class QkNotes : public QWidget
{
    Q_OBJECT

public:
    explicit QkNotes(QWidget *parent = nullptr);
    ~QkNotes() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onNoteBlockTextChanged();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void backup();

private:
    ContentManager m_mgr;
    int m_changeCount;
    QSystemTrayIcon* m_trayIcon;
    bool m_needsRecalcGeometry;
    NoteBlock* m_noteBlock;

    void _initTrayIcon();
    void _saveContent();
    void _recalcGeometryIfNeeded();


    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // QKNOTES_H
