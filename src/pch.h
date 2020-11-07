#ifndef PCH_H
#define PCH_H
#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QTextStream>

extern QString g_dataDir;
extern QSettings g_settings;

static const Qt::KeyboardModifier RESIZE_MOD_KEY = Qt::ControlModifier;
static const Qt::KeyboardModifier SCROOL_MOD_KEY = Qt::ShiftModifier;
static const Qt::KeyboardModifier DRAG_MOD_KEY = Qt::AltModifier;
static const Qt::Key DRAG_KEY = Qt::Key_Alt;

static const QColor SOMENOTES_BG_COLOR = QColor::fromRgb(0xf0, 0xf0, 0xf0);
static const QColor SOMENOTES_DEL_COLOR = QColor::fromRgb(0xff, 0x30, 0x30);
static const QColor SOMENOTES_BASE_COLOR = QColor::fromRgb(0xFD, 0xF3, 0xA7);
static const QColor SOMENOTES_HIGHLIGHT_COLOR = QColor::fromRgb(205, 232, 255);

static const qreal DRAG_THRESHOLD = 0.75;

inline bool IS_AUX_KEY_DOWN(Qt::KeyboardModifier key) { return QApplication::keyboardModifiers().testFlag(key); }

#endif // PCH_H
