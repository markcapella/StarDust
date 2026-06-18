
#include "Global.h"

/**
 * Display helper to check and/or return a valid x11
 * display & session type.
 *
 */
DisplayHelper::DisplayHelper() {
    mDisplay = nullptr;

    // Check for display error.
    const char* WAYLAND_DISPLAY = getenv("WAYLAND_DISPLAY");
    if (WAYLAND_DISPLAY && strlen(WAYLAND_DISPLAY) > 0) {
        cout << XCOLOR_YELLOW << "DisplayHelper: env var "
            "$WAYLAND_DISPLAY: \"" << WAYLAND_DISPLAY <<
            "\"." << XCOLOR_NORMAL << endl;
        cout << XCOLOR_RED << endl << "DisplayHelper: Wayland "
            "Display Manager is detected, FATAL." <<
            XCOLOR_NORMAL << endl;
        return;
    }

    // Check for session error.
    const char* SESSION_TYPE = getenv("XDG_SESSION_TYPE");
    if (SESSION_TYPE && strcmp(SESSION_TYPE, "x11") != 0) {
        cout << XCOLOR_YELLOW << "DisplayHelper: env var "
            "$XDG_SESSION_TYPE: \"" << SESSION_TYPE <<
            "\"." << XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_RED << "DisplayHelper: No X11 "
            "Session type is detected, FATAL." <<
            XCOLOR_NORMAL << endl;
        return;
    }

    // Check for display access.
    mDisplay = XOpenDisplay(NULL);
    if (mDisplay == nullptr) {
        cout << XCOLOR_RED << "DisplayHelper: X11 Display "
            "does not seem to be available (Are you Wayland?) "
            "FATAL." << XCOLOR_NORMAL << endl;
        return;
    }
}

/**
 * Return the Display*.
 */
Display*
DisplayHelper::getDisplay() {

    return mDisplay;
}
