#pragma once

#include <QApplication>
#include <QColor>
#include <QSettings>
#include <QString>
#include <QDebug>

extern QString g_dataDir;

extern QSettings g_settings;
const char *const SETTINGS_NAME_VIEW_MODE = "view_mode";

const QColor SOMENOTES_BG_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);
const QColor SOMENOTES_TRASH_COLOR = QColor::fromRgb(0xff, 0x30, 0x30);
const QColor SOMENOTES_BASE_COLOR = QColor::fromRgb(0xFD, 0xF3, 0xA7);
const QColor SOMENOTES_HIGHLIGHT_COLOR = QColor::fromRgb(205, 232, 255);

const qreal DRAG_THRESHOLD = 0.75;

inline QString CONTENT_PATH() { return g_dataDir + "/content.txt"; }
inline QString TRASH_PATH() { return g_dataDir + "/trash.txt"; }
