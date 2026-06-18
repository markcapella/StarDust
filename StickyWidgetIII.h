
#pragma once

/**
 * First app in the prototype series, an X-based
 * prototype similar to KDE Plasmoids.
 *
 * Desktop widgets with various views can be stuck in
 * place to the desktop below other windows.
 *
 * Widget remembers position, size, settings etc.
 *
 * Cursor hover reveals PinButton that toggles widget
 * in & out of "Stuck" state.
 *
 * When stuck to the desktop, widget ignores clicks &
 * passes all mouse actions to desktop (input transparency),
 * and has a transparent visual background.
 *
 * While "Stuck" to the desktop, the widgets window
 * button in the panels Task manager is removed.
 *
 */

/**
 * Common charsets to enable colorized cout messages.
 * "Ansii escape sequences for cursor color controls".
 */
#define XCOLOR_NORMAL  "\033[0m"

#define XCOLOR_BLACK   "\033[0;30m"
#define XCOLOR_WHITE   "\033[0;37m"

#define XCOLOR_RED     "\033[0;31m"
#define XCOLOR_GREEN   "\033[1;32m"
#define XCOLOR_YELLOW  "\033[1;33m"
#define XCOLOR_BLUE    "\033[1;34m"
#define XCOLOR_MAGENTA "\033[1;35m"
#define XCOLOR_CYAN    "\033[1;36m"

// X11 Render colors (not properly blended).
static inline constexpr XRenderColor TRANSPARENT_RCOLOR =
    { 0x0000, 0x0000, 0x0000, 0x0000 };

static inline constexpr XRenderColor BLACK_RCOLOR =
    { 0x0000, 0x0000, 0x0000, 0xffff };
static inline constexpr XRenderColor WHITE_RCOLOR =
    { 0xffff, 0xffff, 0xffff, 0xffff };

static inline constexpr XRenderColor GRAY_RCOLOR =
    { 0xd800, 0xd500, 0xd200, 0xffff };

static inline constexpr XRenderColor RED_RCOLOR =
    { 0xffff, 0x0000, 0x0000, 0x8000 };
static inline constexpr XRenderColor GREEN_RCOLOR =
    { 0x0000, 0xffff, 0x0000, 0xffff };
static inline constexpr XRenderColor BLUE_RCOLOR =
    { 0x0000, 0x0000, 0xffff, 0xffff };

static inline constexpr XRenderColor TURQUOISE_RCOLOR =
    { 0x0000, 0xffff, 0xfaff, 0xffff };

// x11 Fonts.
static inline constexpr const char*
    TIME_DISPLAY_FONT = "Serif-42:bold";

// Pngs.
static inline const QString PIN_IN_PNG_FILENAME = "pinIn.png";
static inline const QString PIN_OUT_PNG_FILENAME = "pinOut.png";

static inline const string CONFIG_DIALOG_UPDATED_EVENT =
    "CONFIG_DIALOG_UPDATED";

// App globals.
extern DisplayHelper* mDisplayHelper;
extern Display* mDisplay;
extern XHelper* mXHelper;

extern RecentsHelper* mRecentsHelper;
extern SettingsHelper* mSettingsHelper;

extern StickyWindow* mStickyWindow;
extern Canvas* mCanvas;

extern XImage* mPinInXImage;
extern XImage* mPinOutXImage;
