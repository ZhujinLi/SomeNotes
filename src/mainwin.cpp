#include "mainwin.h"
#include "pch.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QMenu>
#include <QProcess>
#include <QScreen>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>
#include <QWindow>
#include <ui_about.h>

#define SETTING_WIDTH "width"
#define SETTING_HEIGHT "height"

MainWin::MainWin(QWidget *parent) : QWidget(parent), m_trayIcon(nullptr) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    layout()->addWidget(scrollArea);

    m_someNotes = new SomeNotes(scrollArea);
    scrollArea->setWidget(m_someNotes);

    _initTrayIcon();

    if (!g_settings.contains(SETTING_WIDTH) || !g_settings.contains(SETTING_HEIGHT)) {
        int w = m_trayIcon->geometry().width() * 10;
        setGeometry(0, 0, w, int(w * 1.5f));
    } else {
        setGeometry(0, 0, g_settings.value(SETTING_WIDTH).toInt(), g_settings.value(SETTING_HEIGHT).toInt());
    }

    m_needsRecalcGeometry = true;
}

void MainWin::_initTrayIcon() {
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/app.png"));

    QMenu *trayIconMenu = new QMenu(this);

    QAction *showAction = new QAction(tr("&Show"), this);
    connect(showAction, &QAction::triggered, this, &MainWin::_appear);
    trayIconMenu->addAction(showAction);

    QAction *backupAction = new QAction(tr("&Backup"), this);
    connect(backupAction, &QAction::triggered, m_someNotes, &SomeNotes::backup);
    trayIconMenu->addAction(backupAction);

    QAction *openAction = new QAction(tr("&Open data directory..."), this);
    connect(openAction, &QAction::triggered, this, &MainWin::_openDataDir);
    trayIconMenu->addAction(openAction);

#ifdef Q_OS_WIN
    m_autoStartAction = new QAction(tr("&Run at startup"), this);
    _updateAutoStartIcon();
    connect(m_autoStartAction, &QAction::triggered, this, &MainWin::_autoStartChanged);
    trayIconMenu->addAction(m_autoStartAction);
#endif

    QAction *restartAction = new QAction(tr("&Restart"), this);
    connect(restartAction, &QAction::triggered, this, &MainWin::_restart);
    trayIconMenu->addAction(restartAction);

    QAction *aboutAction = new QAction(tr("&About..."), this);
    connect(aboutAction, &QAction::triggered, this, &MainWin::_about);
    trayIconMenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWin::iconActivated);

    m_trayGeo = trayIcon->geometry();

    trayIcon->show();

    m_trayIcon = trayIcon;
}

void MainWin::_recalcGeometryIfNeeded() {
    if (!m_needsRecalcGeometry || m_trayIcon == nullptr)
        return;
    m_needsRecalcGeometry = false;

    QRect trayGeometry = m_trayIcon->geometry();
    int w = width();
    int h = height();

    bool isTrayAtTop = trayGeometry.y() == 0;
    if (isTrayAtTop) {
        setGeometry(trayGeometry.center().x() - w / 2, trayGeometry.bottom() + trayGeometry.height() / 4, w, h);
    } else {
        setGeometry(trayGeometry.center().x() - w / 2, trayGeometry.top() - trayGeometry.height() / 4 - h, w, h);
    }

    g_settings.setValue(SETTING_WIDTH, width());
    g_settings.setValue(SETTING_HEIGHT, height());
}

#ifdef Q_OS_WIN
static const QString s_appNameInRun = "SomeNotes";
static const QString s_runInReg = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

static QString _nativeAppPath() {
    QString path = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    return "\"" + path + "\""; // In case there are spaces in the path
}

bool MainWin::_isAutoStart() {
    QSettings runSettings(s_runInReg, QSettings::NativeFormat);
    bool contains = runSettings.contains(s_appNameInRun);
    if (contains && runSettings.value(s_appNameInRun).toString().compare(_nativeAppPath())) {
        qWarning() << "Another auto start instance, update it...";
        runSettings.setValue(s_appNameInRun, _nativeAppPath());
    }
    return contains;
}

void MainWin::_autoStartChanged() {
    QSettings runSettings(s_runInReg, QSettings::NativeFormat);
    if (_isAutoStart())
        runSettings.remove(s_appNameInRun);
    else
        runSettings.setValue(s_appNameInRun, _nativeAppPath());
    _updateAutoStartIcon();
}

void MainWin::_updateAutoStartIcon() {
    m_autoStartAction->setIcon(_isAutoStart() ? QIcon(":/images/checked.png") : QIcon(":/images/unchecked.png"));
}
#endif

void MainWin::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    case Qt::Key_Right:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() - 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Left:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width() + 10, height());
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Down:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() - 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case Qt::Key_Up:
        if (IS_AUX_KEY_DOWN(RESIZE_MOD_KEY)) {
            this->setGeometry(geometry().x(), geometry().y(), width(), height() + 10);
            m_needsRecalcGeometry = true;
        }
        break;
    case DRAG_KEY:
        QApplication::restoreOverrideCursor();
        break;
    default:
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void MainWin::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}

void MainWin::_openDataDir() { QDesktopServices::openUrl(QUrl::fromLocalFile(g_dataDir)); }

void MainWin::_about() {
    QDialog *dialog = new QDialog(this);
    Ui::About aboutUi;
    aboutUi.setupUi(dialog);
    dialog->show();

    QSize size = dialog->size();
    qreal h = (aboutUi.textBrowser->document()->size().height() + dialog->layout()->margin() * 2) * 1.2;
    size.setWidth(static_cast<int>(h * size.width() / size.height()));
    size.setHeight(static_cast<int>(h));
    dialog->setFixedSize(size);

    QScreen *screen = dialog->window()->windowHandle()->screen();
    dialog->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, dialog->size(), screen->geometry()));
}

void MainWin::_restart() {
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWin::_appear() {
    show();
    raise();
    activateWindow();
}

void MainWin::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        _appear();
        break;
    default:;
    }
}

bool MainWin::event(QEvent *event) {
    if (m_trayIcon && m_trayIcon->geometry() != m_trayGeo) {
        m_trayGeo = m_trayIcon->geometry();
        m_needsRecalcGeometry = true;
    }
    _recalcGeometryIfNeeded();
    return QWidget::event(event);
}
