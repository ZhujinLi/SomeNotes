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
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void backup();
    void placeholderTextChanged();
    void onNoteBlockNoteDeleted(NoteBlock* noteBlock);
    void onNoteBlockTrySwap(NoteBlock* noteBlock);

private:
    ContentManager m_mgr;
    QSystemTrayIcon* m_trayIcon;
    bool m_needsRecalcGeometry;
    std::vector<NoteBlock*> m_noteBlocks;
    NoteBlockPlaceholder* m_placeholder;

    void _initTrayIcon();
    void _recalcGeometryIfNeeded();
    void _focusToNoteBlock(QPlainTextEdit* noteBlock);
    NoteBlock* _addNoteBlock(NoteBlockContent* content);

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
