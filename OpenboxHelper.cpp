
#include "Global.h"

/**
 * OpenboxHelper addresses differences in xfwm4 / kwin / mutter
 * Window Manager behaviors. Openbox has quirks for windows that
 * are DOCKS (undecorated windows such as StickWidgets) ...
 * ... in its "Stay on Top" or "Stay on Bottom" logic.
 *
 */
OpenboxHelper::OpenboxHelper() {
}

/**
 * Openbox won't move an undecorated DOCK WINDOW type to
 * Top or Bottom, so we do it ourselves.
 */
void
OpenboxHelper::setWindowOnTopOrOnBottomForOpenbox(const Window window) {
    // Early exit if not Openbox.
    if (mXHelper->getWindowManagerName() != "Openbox") {
        return;
    }

    // Set Top or Bottom as appropriate.
    if (mSettingsHelper->getBoolSetting(
        SettingsHelper::ON_TOP_INSTEAD)) {
        setWindowOnTopForOpenbox(window);
        return;
    }

    setWindowOnBottomForOpenbox(window);
}

/**
 * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
 * Top or Bottom, so we do it ourselves.
 */
void
OpenboxHelper::ensureOnTopOrOnBottomForOpenbox(const Window window) {
    // Early exit if not Openbox.
    if (mXHelper->getWindowManagerName() != "Openbox") {
        return;
    }

    // Ensure STAY ON Top or Bottom as appropriate.
    if (mSettingsHelper->getBoolSetting(
        SettingsHelper::ON_TOP_INSTEAD)) {
        ensureOnTopForOpenbox(window);
        return;
    }

    ensureOnBottomForOpenbox(window);
}

/**
 * Openbox won't move an undecorated DOCK WINDOW type to
 * Top, so we do it ourselves.
 */
void
OpenboxHelper::setWindowOnTopForOpenbox(const Window window) {

    XRaiseWindow(mDisplay, window);
}

/**
 * Openbox won't move an undecorated DOCK WINDOW type to
 * Bottom, so we do it ourselves.
 */
void
OpenboxHelper::setWindowOnBottomForOpenbox(const Window window) {

    XLowerWindow(mDisplay, window);
}

/**
 * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
 * Top, so we do it ourselves.
 */
void
OpenboxHelper::ensureOnTopForOpenbox(const Window window) {
    // Lower all windows above us that 1) Don't stick to top,
    // and 2) Are not FOCUSED & in fullscreen mode.
    const vector<Window> windows = mXHelper->getWindowsStackedList();

    for (int i = windows.size() - 1; i > 0; i--) {
        // When we find us, there's nothing left above us
        // that doesn't belong there.
        if (window == windows[i]) {
            return;
        }

        // If window above us stays on top, leave it there.
        if (mXHelper->doesWindowStayOnTop(windows[i])) {
            continue;
        }

        // If window above us stays fullscreeen, remove focus
        // from single fullscreen app, set to us, move us to top,
        // & we're done.
        if (mXHelper->doesWindowStayFullScreen(windows[i])) {
            XSetInputFocus(mDisplay, window, RevertToPointerRoot,
                CurrentTime);
            XRaiseWindow(mDisplay, window);
            return;
        }

        // Else, lower non-on-top window to be under us.
        XLowerWindow(mDisplay, windows[i]);
    }
}

/**
 * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
 * Bottom, so we do it ourselves.
 */
void
OpenboxHelper::ensureOnBottomForOpenbox(const Window window) {
    // Raise all windows under us that don't stick to bottom.
    const vector<Window> windows = mXHelper->getWindowsStackedList();

    for (int i = 1; i < windows.size(); i++) {
        // When we find us, there's nothing left under us
        // that doesn't belong there.
        if (window == windows[i]) {
            return;
        }

        // If window under us stays on bottom, leave it there.
        if (mXHelper->doesWindowStayOnBottom(windows[i])) {
            continue;
        }

        // Else, raise window under us.
        XRaiseWindow(mDisplay, windows[i]);
    }
}
