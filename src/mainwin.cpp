#include "mainwin.h"
#include "common.h"
#include "ui_about.h"
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

MainWin::MainWin() : QWidget(nullptr), m_trayIcon(nullptr) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    layout()->addWidget(scrollArea);

    m_notesView = new NotesView(scrollArea);
    scrollArea->setWidget(m_notesView);

    _initTrayIcon();

    ViewMode viewMode = ViewMode::Portrait;
    if (g_settings.contains(SETTINGS_NAME_VIEW_MODE))
        viewMode = static_cast<ViewMode>(g_settings.value(SETTINGS_NAME_VIEW_MODE).toInt());
    _setViewMode(viewMode);
}

void MainWin::_initTrayIcon() {
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);

#ifdef Q_OS_MACOS
    trayIcon->setIcon(QIcon(":/images/app_s.png"));
#else
    trayIcon->setIcon(QIcon(":/images/app.png"));
#endif

    QMenu *trayIconMenu = new QMenu(this);

    QAction *showAction = new QAction(tr("&Show"), this);
    connect(showAction, &QAction::triggered, this, &MainWin::_appear);
    trayIconMenu->addAction(showAction);

#ifdef Q_OS_WIN
    m_autoStartAction = new QAction(tr("&Run at startup"), this);
    _updateAutoStartIcon();
    connect(m_autoStartAction, &QAction::triggered, this, &MainWin::_autoStartChanged);
    trayIconMenu->addAction(m_autoStartAction);
#endif

    trayIconMenu->addSeparator();

    m_portraitAction = new QAction(tr("&Portrait"), this);
    connect(m_portraitAction, &QAction::triggered, this, &MainWin::_onPortraitModeSelected);
    trayIconMenu->addAction(m_portraitAction);

    m_landscapeAction = new QAction(tr("&Landscape"), this);
    connect(m_landscapeAction, &QAction::triggered, this, &MainWin::_onLandscapeModeSelected);
    trayIconMenu->addAction(m_landscapeAction);

    trayIconMenu->addSeparator();

    QAction *trashAction = new QAction(tr("&Trash..."), this);
    connect(trashAction, &QAction::triggered, this, &MainWin::_gotoTrashFile);
    trayIconMenu->addAction(trashAction);

    QAction *aboutAction = new QAction(tr("&About..."), this);
    connect(aboutAction, &QAction::triggered, this, &MainWin::_about);
    trayIconMenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWin::_onIconActivated);

    m_trayGeo = trayIcon->geometry();

    trayIcon->show();

    m_trayIcon = trayIcon;
}

void MainWin::_recalcGeometryIfNeeded() {
    if (!m_needsRecalcGeometry || m_trayIcon == nullptr)
        return;
    m_needsRecalcGeometry = false;

    // Calculate size according to view mode
    QSize size;
    switch (static_cast<ViewMode>(g_settings.value(SETTINGS_NAME_VIEW_MODE).toInt())) {
    case ViewMode::Portrait:
        size = QSize(350, 500);
        break;
    case ViewMode::Landscape:
    default:
        size = QSize(800, 400);
        break;
    }

    // Centered at the tray icon position
    QRect trayGeometry = m_trayIcon->geometry();
    int gap = trayGeometry.height() / 4;

    int w = size.width();
    int h = size.height();

    QPoint leftTop;
    if (trayGeometry.y() == 0) { // The tray is on the top
        leftTop = QPoint(trayGeometry.center().x() - w / 2, trayGeometry.bottom() + gap);
    } else {
        leftTop = QPoint(trayGeometry.center().x() - w / 2, trayGeometry.top() - gap - h);
    }

    // Make sure the area is inside the screen
    int screenWidth = window()->windowHandle()->screen()->size().width();
    if (leftTop.x() + w > screenWidth - gap) {
        leftTop.setX(screenWidth - gap - w);
    }

    setGeometry(QRect(leftTop, size));
}

void MainWin::_onPortraitModeSelected() {
    _setViewMode(ViewMode::Portrait);
    _recalcGeometryIfNeeded();
}

void MainWin::_onLandscapeModeSelected() {
    _setViewMode(ViewMode::Landscape);
    _recalcGeometryIfNeeded();
}

void MainWin::_setViewMode(ViewMode viewMode) {
    // Save to disk
    g_settings.setValue(SETTINGS_NAME_VIEW_MODE, static_cast<int>(viewMode));

    // Update menu icons
    m_portraitAction->setIcon(viewMode == ViewMode::Portrait ? QIcon(":/images/selected.png")
                                                             : QIcon(":/images/unselected.png"));
    m_landscapeAction->setIcon(viewMode == ViewMode::Landscape ? QIcon(":/images/selected.png")
                                                               : QIcon(":/images/unselected.png"));

    m_needsRecalcGeometry = true;
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
    default:
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void MainWin::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}

void MainWin::_gotoTrashFile() { QDesktopServices::openUrl(QUrl::fromLocalFile(TRASH_PATH())); }

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

void MainWin::_appear() {
    show();
    raise();
    activateWindow();
}

void MainWin::_onIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        _appear();
        break;
    default:;
    }
}

void MainWin::showEvent(QShowEvent *event) {
    if (m_trayIcon != nullptr && m_trayIcon->geometry() != m_trayGeo) {
        m_trayGeo = m_trayIcon->geometry();
        m_needsRecalcGeometry = true;
    }

    _recalcGeometryIfNeeded();

    QWidget::showEvent(event);
}
