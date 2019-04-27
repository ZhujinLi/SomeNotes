#include "pch.h"
#include "qknotes.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QLayout>
#include <QMenu>

#define SETTING_WIDTH "width"
#define SETTING_HEIGHT "height"

QkNotes::QkNotes(QWidget *parent) :
    QWidget(parent)
{
    m_needsRecalcGeometry = false;

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    _initTrayIcon();

    if (!g_settings.contains(SETTING_WIDTH) || !g_settings.contains(SETTING_HEIGHT)) {
        int w = m_trayIcon->geometry().width() * 10;
        setGeometry(0, 0, w, int(w * 1.5f));
    } else {
        setGeometry(0, 0, g_settings.value(SETTING_WIDTH).toInt(), g_settings.value(SETTING_HEIGHT).toInt());
    }

    QLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    for (size_t i = 0; i < m_mgr.getContentCount(); i++) {
        NoteBlockContent* content = m_mgr.getContent(i);
        _addNoteBlock(content);
    }

    m_placeholder = new NoteBlockPlaceholder(this);
    layout->addWidget(m_placeholder);
    connect(m_placeholder, &QPlainTextEdit::textChanged, this, &QkNotes::placeholderTextChanged);

    show();
    activateWindow();
    if (m_noteBlocks.size())
        _focusToNoteBlock(m_noteBlocks[0]);
    else
        _focusToNoteBlock(m_placeholder);

    m_needsRecalcGeometry = true;
}

void QkNotes::_initTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/images/tray.png"));

    QMenu* trayIconMenu = new QMenu(this);

    QAction* backupAction = new QAction(tr("&Backup"), this);
    connect(backupAction, &QAction::triggered, this, &QkNotes::backup);
    trayIconMenu->addAction(backupAction);

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &QkNotes::iconActivated);

    m_trayIcon->show();
}

QkNotes::~QkNotes()
{
    // since mgr is shared
    for (NoteBlock* note : m_noteBlocks)
        delete note;
    delete layout();
}

void QkNotes::_recalcGeometryIfNeeded()
{
    if (!m_needsRecalcGeometry)
        return;
    m_needsRecalcGeometry = false;

    QRect trayGeometry = m_trayIcon->geometry();
    int w = width();
    int h = height();

    bool isTrayAtTop = trayGeometry.y() == 0;
    if (isTrayAtTop) {
        setGeometry(trayGeometry.center().x() - w / 2,
                    trayGeometry.bottom() + trayGeometry.height() / 4,
                    w, h);
    } else {
        setGeometry(trayGeometry.center().x() - w / 2,
                    trayGeometry.top() - trayGeometry.height() / 4 - h,
                    w, h);
    }

    g_settings.setValue(SETTING_WIDTH, width());
    g_settings.setValue(SETTING_HEIGHT, height());
}

void QkNotes::_focusToNoteBlock(QPlainTextEdit* noteBlock)
{
    noteBlock->setFocus();
    QTextCursor cursor = noteBlock->textCursor();
    cursor.movePosition(QTextCursor::End);
    noteBlock->setTextCursor(cursor);
}

NoteBlock *QkNotes::_addNoteBlock(NoteBlockContent *content)
{
    NoteBlock* noteBlock = new NoteBlock(content, this);
    connect(noteBlock, &NoteBlock::noteDeleted, this, &QkNotes::onNoteBlockNoteDeleted);
    m_noteBlocks.push_back(noteBlock);
    layout()->addWidget(noteBlock);
    return noteBlock;
}

void QkNotes::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    case Qt::Key_Minus:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() - 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Equal:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() + 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Underscore:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Plus:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() + 10);
            m_needsRecalcGeometry = true;
        }
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void QkNotes::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void QkNotes::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        show();
        activateWindow();
        break;
    default:
        ;
    }
}

void QkNotes::backup()
{
    for (NoteBlock* noteBlock : m_noteBlocks)
        noteBlock->saveContent();
    m_mgr.backup();
}

void QkNotes::placeholderTextChanged()
{
    const QString& text = m_placeholder->toPlainText();
    if (!text.isEmpty()) {
        layout()->removeWidget(m_placeholder);

        NoteBlockContent* content = m_mgr.newContent();
        content->setText(text);

        NoteBlock* noteBlock = _addNoteBlock(content);

        layout()->addWidget(m_placeholder);

        m_placeholder->setPlainText("");

        _focusToNoteBlock(noteBlock);
    }
}

void QkNotes::onNoteBlockNoteDeleted(NoteBlock *noteBlock)
{
    for (size_t i = 0; i < m_noteBlocks.size(); i++)
        if (m_noteBlocks[i] == noteBlock) {
            layout()->removeWidget(noteBlock);
            m_noteBlocks.erase(m_noteBlocks.begin() + static_cast<long long>(i));
            NoteBlockContent* content = noteBlock->getContent();
            delete noteBlock;
            m_mgr.deleteContent(content);
            return;
        }
}

bool QkNotes::event(QEvent *event)
{
    _recalcGeometryIfNeeded();
    return QWidget::event(event);
}

