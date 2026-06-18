
#pragma once

/**
 * Global headers, order important.
 *
 */
// Standard C libraries.
#include <chrono>
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
typedef chrono::steady_clock Clock;

// Qt libraries.
#include <QApplication>
#include <QSettings>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QMouseEvent>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QToolTip>
#include <QStyleOptionFrame>
#include <QPointer>

// x11 libraries.
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>

// libpng library.
#include <png.h>

// libasan library.
#include <sanitizer/lsan_interface.h>
#include <sys/resource.h>

// Application libraries.
#include "AutoHideDelayHints.h"
#include "DesktopPreferenceHints.h"
#include "OpacityHints.h"
#include "SaturationHints.h"
#include "MaxStarSizeHints.h"

#include "ColorChangeRateHints.h"
#include "SizeChangeRateHints.h"
#include "PositionChangeRateHints.h"

#include "Star.h"
#include "Canvas.h"

#include "Button.h"
#include "PinButton.h"
#include "QuitButton.h"
#include "MoveButton.h"
#include "SizeButton.h"

#include "ConfigDialog.h"
#include "ColorButton.h"
#include "AboutDialog.h"
#include "ConfigButton.h"

#include "StickyWindow.h"

#include "SettingsHelper.h"
#include "RecentsHelper.h"
#include "DisplayHelper.h"

#include "WinInfo.h"
#include "XHelper.h"

#include "StickyWidgetIII.h"

// Utiliy Macros.
#define randomIntegerUpTo(n) \
    ((int) (((n) <= 0) ? 0.0 : (drand48() * (n))))

#define newRenderColor(r, g, b, a) \
    { (unsigned short)(((r) * (a)) / 255), \
      (unsigned short)(((g) * (a)) / 255), \
      (unsigned short)(((b) * (a)) / 255), \
      (unsigned short)((a) * 257) }
