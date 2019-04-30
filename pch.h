#ifndef PCH_H
#define PCH_H
#include <QString>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QApplication>

extern QString g_dataDir;
extern QSettings g_settings;

static const Qt::KeyboardModifier RESIZE_MOD_KEY = Qt::ControlModifier;
static const Qt::KeyboardModifier SCROOL_MOD_KEY = Qt::ShiftModifier;
static const Qt::KeyboardModifier DRAG_MOD_KEY = Qt::AltModifier;
static const Qt::Key DRAG_KEY = Qt::Key_Alt;

static const qreal DRAG_THRESHOLD = 0.75;

inline bool IS_AUX_KEY_DOWN(Qt::KeyboardModifier key) {
    return QApplication::keyboardModifiers().testFlag(key);
}

#endif // PCH_H
