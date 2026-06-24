
#include "Global.h"

/**
 * XHelper provides common X related methods.
 */
XHelper::XHelper() {
    if (!mDisplay) {
        return;
    }

    mAtomDMSupportsWMCheck = XInternAtom(mDisplay,
        "_NET_SUPPORTING_WM_CHECK", False);
    mAtomGetWMName = XInternAtom(mDisplay,
        "_NET_WM_NAME", False);
    mAtomGetUTF8String = XInternAtom(mDisplay,
        "UTF8_STRING", False);
}

/**
 * This method traps and handles X11 errors.
 */
int
XHelper::handleX11ErrorEvent(Display* display,
    XErrorEvent* event) {
    if (event->error_code == BadWindow ||
        event->error_code == BadAccess ||
        event->error_code == BadMatch) {
        return 0;
    }

    // Print the error message of the event.
    const int MAX_MESSAGE_BUFFER_LENGTH = 4096;
    char msg[MAX_MESSAGE_BUFFER_LENGTH];
    XGetErrorText(display, event->error_code, msg,
        sizeof(msg));

    printf("%sHandleX11ErrorEvent() %s.%s\n",
        XCOLOR_RED, msg, XCOLOR_NORMAL);
    return 1;
}

/**
 * This method returns the Window Managers name.
 */
string
XHelper::getWindowManagerName() {
    if (!canDisplayReportWMName()) {
        return "";
    }

    const Window ROOT_WINDOW = getRootWindowFromDisplay();
    if (!ROOT_WINDOW) {
        return "";
    }

    const string WM_NAME = getWMNameFromRootWindow(
        ROOT_WINDOW);
    return WM_NAME == "GNOME Shell" ?
        "Gnome Shell (Mutter)" : WM_NAME;
}

/**
 * Helper method to check if the DM can report
 * information about the WM.
 */
bool
XHelper::canDisplayReportWMName() {
    return mAtomDMSupportsWMCheck && mAtomGetWMName &&
        mAtomGetUTF8String;
}

/**
 * Helper method to get the Root Window of the DM.
 */
Window
XHelper::getRootWindowFromDisplay() {
    Atom resultType;
    int resultFormat;
    unsigned long resultCount;
    unsigned long unused;

    unsigned char* resultWindowPtr = nullptr;
    if (XGetWindowProperty(mDisplay, DefaultRootWindow(mDisplay),
        mAtomDMSupportsWMCheck, 0, 1, False, XA_WINDOW,
        &resultType, &resultFormat, &resultCount,
        &unused, &resultWindowPtr) == Success) {

        if (resultWindowPtr != nullptr) {
            if (resultType == XA_WINDOW &&
                resultFormat == 32 && resultCount == 1) {
                const Window ROOT_WINDOW = *reinterpret_cast
                    <Window*> (resultWindowPtr);
                XFree(resultWindowPtr);
                resultWindowPtr = nullptr;
                return ROOT_WINDOW;
            }
            XFree(resultWindowPtr);
            resultWindowPtr = nullptr;
        }
    }

    return None;
}

/**
 * Helper method to get the WM Name from the
 * DM's Root Window.
 */
string
XHelper::getWMNameFromRootWindow(const Window rootWindow) {
    if (rootWindow == None) {
        return "";
    }

    Atom resultType;
    int resultFormat;
    unsigned long resultCount;
    unsigned long unused;

    unsigned char* resultWindowPtr = nullptr;
    if (XGetWindowProperty(mDisplay, rootWindow, mAtomGetWMName,
        0, 1024, False, mAtomGetUTF8String, &resultType, &resultFormat,
        &resultCount, &unused, &resultWindowPtr) == Success) {

        if (resultWindowPtr != nullptr) {
            if (resultType == mAtomGetUTF8String ||
                resultType == XA_STRING) {
                const string WM_NAME(reinterpret_cast
                    <char*> (resultWindowPtr), resultCount);
                XFree(resultWindowPtr);
                resultWindowPtr = nullptr;
                return WM_NAME;
            }
        }
    }

    return "";
}

/**
 * This method determines if a compositor is running.
 */
bool
XHelper::isACompositorRunning() {
    // Check for the Composite Extension.
    int dummy1, dummy2;
    if (!XCompositeQueryExtension(mDisplay, &dummy1, &dummy2)) {
        return false;
    }

    Atom composite_atom;
    composite_atom = XInternAtom(mDisplay, "_NET_WM_CM_S0", True);
    if (composite_atom != None) {
        return true;
    }
    if (XGetSelectionOwner(mDisplay, composite_atom) != None) {
        return true;
    }

    return false;
}

/**
 * This method determines the Compositors name.
 */
QString
XHelper::getCompositorName() {
    const string ATOM_NAME = "_NET_WM_CM_S" +
        to_string(DefaultScreen(mDisplay));
    const Atom CM_SELECTION = XInternAtom(mDisplay,
        ATOM_NAME.c_str(), False);

    // 2. Find the window that owns this selection
    const Window WINDOW_OWNER = XGetSelectionOwner(
        mDisplay, CM_SELECTION);
    if (WINDOW_OWNER == None) {
        return QString("");
    }

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char* prop_name = nullptr;

    const Atom UTF8_STRING = XInternAtom(mDisplay, "UTF8_STRING", False);
    if (XGetWindowProperty(mDisplay, WINDOW_OWNER, mAtomGetWMName,
        0, 1024, False, UTF8_STRING, &actual_type, &actual_format, 
        &nitems, &bytes_after, &prop_name) == Success && prop_name) {
        string name(reinterpret_cast<char*>(prop_name));
        XFree(prop_name);
        return QString(name.c_str());
    }

    // Fallback to standard WM_NAME if _NET_WM_NAME isn't set
    char* classic_name = nullptr;
    if (XFetchName(mDisplay, WINDOW_OWNER, &classic_name) &&
        classic_name) {
        string name(classic_name);
        XFree(classic_name);
        return QString(name.c_str());
    }

    return QString("");
}

/**
 * Check if display supports TrueColor32 visual transparency.
 */
bool
XHelper::isTransparentVisually() {
    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        return false;
    }

    XVisualInfo visualInfoStruct{};
    const int VISUAL_COLOR_DEPTH = 32;
    if (XMatchVisualInfo(mDisplay, DefaultScreen(mDisplay),
        VISUAL_COLOR_DEPTH, TrueColor, &visualInfoStruct)) {
        return true;
    }

    return false;
}

/**
 * Check if the display supports transparent pointer events.
 */
bool
XHelper::isTransparentToPointer() {
    // Check for the Composite Extension.
    int dummy1, dummy2;
    if (!XCompositeQueryExtension(mDisplay, &dummy1, &dummy2)) {
        return false;
    }

    // Check for version >= 1.1 (required for ShapeInput).
    int major, minor;
    if (XShapeQueryVersion(mDisplay, &major, &minor)) {
        return (major > 1 || (major == 1 && minor >= 1));
    }

    return false;
}

/**
 * This method checks if the desktop is currently
 * being shown, (which hides all windows).
 */
bool
XHelper::isDesktopShowing() {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay,
        "_NET_SHOWING_DESKTOP", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32 && nItems > 0 &&
        (*(long*) (void*) properties == 1)) {
        XFree(properties);
        return true;
    }
    XFree(properties);

    return false;
}

/**
 * This method returns the number of the current workspace,
 * Where the OS allows multiple / virtual workspaces.
 *
 * Result == -1 is one big workspace is visible (Viewport).
 */
long
XHelper::getVisibleDesktop() {
    XFlush(mDisplay);

    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay, "_NET_CURRENT_DESKTOP",
        False), 0, 1, False, AnyPropertyType, &type, &format,
        &nItems, &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL) {
        const long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }

    XFree(properties);
    return -1;
}

/**
 * This method changes the users visible desktop.
 */
void
XHelper::setVisibleDesktop(const long desktop) {
    const Atom MESSAGE = XInternAtom(mDisplay,
        "_NET_CURRENT_DESKTOP", False);

    XEvent event{};
    event.type = ClientMessage;
    event.xclient.window = DefaultRootWindow(mDisplay);
    event.xclient.message_type = MESSAGE;

    event.xclient.format = 32;
    event.xclient.data.l[0] = desktop;
    event.xclient.data.l[1] = CurrentTime; 
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;

    XSendEvent(mDisplay, DefaultRootWindow(mDisplay), False,
        SubstructureNotifyMask | SubstructureRedirectMask, &event);
    XFlush(mDisplay);
}

/**
 * This method returns the maximum number of allowable
 * workspaces, where the OS allows multiple / virtual workspaces.
 *
 * Result == -1 means one big workspace is visible (Viewport).
 */
long
XHelper::getMaximumDesktops() {
    long result = 1;

    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    const int RESULT = XGetWindowProperty(mDisplay, DefaultRootWindow(
        mDisplay), XInternAtom(mDisplay, "_NET_NUMBER_OF_DESKTOPS",
        False), 0L, 1L, False, XA_CARDINAL, &type, &format, &nItems,
        &unusedBytes, &properties);
    if (RESULT == Success && properties != nullptr &&
        type == XA_CARDINAL && nItems > 0) {
        result = static_cast<int>(*(long*) properties);
    }

    XFree(properties);
    return result;
}

/**
 * Method returns a a list of active X11 windows
 * in stacking order.
 */
vector<Window>
XHelper::getWindowsStackedList() {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    vector<Window> result;
    if (XGetWindowProperty(mDisplay, DefaultRootWindow(mDisplay),
        XInternAtom(mDisplay, "_NET_CLIENT_LIST_STACKING", False),
        0L, 1024, False, XA_WINDOW, &type, &format, &nItems,
        &unusedBytes, &properties) == Success) {
        Window* windows = (Window*) properties;
        // 0 is desktop, add up.
        for (unsigned long i = 0; i < nItems; i++) {
            result.push_back(windows[i]);
        }
    }
    XFree(properties);

    return result;
}

/**
 * Method returns if a window is in the list
 * of active X11 windows.
 */
bool
XHelper::isWindowInStackedList(const Window window) {
    const vector<Window> windows = getWindowsStackedList();
    const int WINDOWS_SIZE = windows.size();

    // 0 is desktop, search order unimportant.
    for (int i = 0; i < WINDOWS_SIZE; i++) {
        if (windows[i] == window) {
            return true;
        }
    }
    return false;
}

/**
 * This method waits until a window is in the list
 * of active X11 windows.
 */
bool
XHelper::waitForWindowInStackedList(const Window window,
    const int maxWaitTimeMS) {
    if (isWindowInStackedList(window)) {
        return true;
    }

    const chrono::milliseconds MAX_WAIT_TIME(maxWaitTimeMS);
    const chrono::milliseconds DELAY_TIME(10);
    chrono::milliseconds timeWaited(0);

    while (timeWaited < MAX_WAIT_TIME) {
        if (isWindowInStackedList(window)) {
            return true;
        }
        this_thread::sleep_for(DELAY_TIME);
        timeWaited += DELAY_TIME;
    }
    return false;
}

/**
 * This method waits until a window is in the list of
 * active X11 windows and mapped or unmapped as requested.
 */
bool
XHelper::waitForWindowMapState(const Window window,
    const MapState mapState, const int maxWaitTimeMS) {
    WinInfo* winInfo = getWinInfoForWindow(window);
    if (winInfo && winInfo->mapState == mapState) {
        delete winInfo;
        return true;
    }

    const chrono::milliseconds MAX_WAIT_TIME(maxWaitTimeMS);
    const chrono::milliseconds DELAY_TIME(10);
    chrono::milliseconds timeWaited(0);

    while (timeWaited < MAX_WAIT_TIME) {
        winInfo = getWinInfoForWindow(window);
        if (winInfo && winInfo->mapState == mapState) {
            delete winInfo;
            return true;
        }
        this_thread::sleep_for(DELAY_TIME);
        timeWaited += DELAY_TIME;
    }

    if (winInfo) {
        delete winInfo;
    }
    return false;
}

/**
 * This method waits until a window is in the list of
 * active X11 windows and @ requested position.
 */
bool
XHelper::waitForWindowMove(const Window window,
    const QPoint position, const int maxWaitTimeMS) {
    const chrono::milliseconds MAX_WAIT_TIME(maxWaitTimeMS);
    const chrono::milliseconds DELAY_TIME(10);
    chrono::milliseconds timeWaited(0);

    // Wait allows move to same position.
    this_thread::sleep_for(DELAY_TIME);

    while (timeWaited < MAX_WAIT_TIME) {
        WinInfo* winInfo = getWinInfoForWindow(window);
        if (winInfo &&
            winInfo->windowRect.x() == position.x() &&
            winInfo->windowRect.y() == position.y()) {
            delete winInfo;
            return true;
        }
        delete winInfo;
        this_thread::sleep_for(DELAY_TIME);
        timeWaited += DELAY_TIME;
    }
    return false;
}

/**
 * Getter to return WinInfo* for a Window.
 */
int
XHelper::getWindowStackNumber(const Window window) {
    vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // 0 is desktop, search order unimportant.
    int result = -1;
    for (int i = 0; i < WININFO_SIZE; i++) {
        WinInfo* winInfo = winInfos[i];
        if (winInfo->window == window) {
            result = i;
            delete winInfos[i];
        }
    }

    winInfos.clear();
    return result;
}

/**
 * Getter for Window Position.
 */
QPoint
XHelper::getWindowPosition(const Window window) {
    int xCoord = 0;
    int yCoord = 0;
    Window unused;

    XTranslateCoordinates(mDisplay, window, DefaultRootWindow(
        mDisplay), 0, 0, &xCoord, &yCoord, &unused);

    return QPoint(xCoord, yCoord);
}

/**
 * Getter for Window Size.
 */
QSize
XHelper::getWindowSize(const Window window) {
    XWindowAttributes wAttrs;
    XGetWindowAttributes(mDisplay, window, &wAttrs);

    return QSize(wAttrs.width, wAttrs.height);
}

/**
 * Getter for Decorated Window Size.
 */
QSize
XHelper::getWindowFrameOffset(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    const int CALL_RESULT = XGetWindowProperty(mDisplay, window,
        XInternAtom(mDisplay, "_NET_FRAME_EXTENTS", False),
        0, 4, False, XA_CARDINAL, &type, &format, &nItems,
        &unusedBytes, &properties);

    if (CALL_RESULT == Success &&
        properties != nullptr && nItems == 4) {
        const long* DATA = reinterpret_cast<long*> (properties);
        const QSize RESULT(DATA[0], DATA[2]);
        XFree(properties);
        return RESULT;
    }

    XFree(properties);
    return QSize(0, 0);
}

/**
 * Getter for Window Mapstate.
 */
int
XHelper::getWindowMapstate(const Window window) {
    XWindowAttributes wAttrs;
    XGetWindowAttributes(mDisplay, window, &wAttrs);

    return wAttrs.map_state;
}

/**
 * Gets window PID.
 */
pid_t
XHelper::getWindowPID(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    // Request the property from the X server
    const int RESULT = XGetWindowProperty(mDisplay, window,
        XInternAtom(mDisplay, "_NET_WM_PID", True), 0, 1024, False,
        AnyPropertyType, &type, &format, &nItems, &unusedBytes,
        &properties);

    unsigned long pid = 0;
    pid = (RESULT == Success && properties != nullptr) ?
        *((unsigned long*) properties) : 0;

    XFree(properties);
    return pid;
}

/**
 * Sets window PID.
 */
void
XHelper::setWindowPID(const Window window) {
    const pid_t PID = getpid();

    XChangeProperty(mDisplay, window, XInternAtom(mDisplay,
        "_NET_WM_PID", False), XA_CARDINAL, 32, PropModeReplace,
        (unsigned char*) &PID, 1);
    XFlush(mDisplay);
}

/**
 * Sets window titlebar, border to desired visibility.
 */
void
XHelper::setWindowType(const Window window,
    const Atom windowType) {
    const Atom WINDOW_TYPE = XInternAtom(mDisplay,
        "_NET_WM_WINDOW_TYPE", false);

    XChangeProperty(mDisplay, window, WINDOW_TYPE, XA_ATOM, 32,
        PropModeReplace, (unsigned char*) &windowType, 1);
    XFlush(mDisplay);
}

/**
 * This method returns a 40-char window title string.
 *    Replace unprintables with SPACE.
 */
string
XHelper::getWindowTitle(const Window window) {
    const int TITLE_LENGTH = 40;
    char mTitleOfWindow[TITLE_LENGTH + 1] = "";

    XTextProperty titleBarName;
    int outP = 0;

    if (XGetWMName(mDisplay, window, &titleBarName)) {
        const char* NAME_PTR = (char*) titleBarName.value;
        const int NAME_LEN = strlen(NAME_PTR);

        for (; outP < NAME_LEN && outP < TITLE_LENGTH; outP++) {
            mTitleOfWindow[outP] = isprint(*(NAME_PTR + outP)) ?
                *(NAME_PTR + outP) : ' ';
        }
        XFree(titleBarName.value);
    }

    for (; outP < TITLE_LENGTH; outP++) {
        mTitleOfWindow[outP] = ' ';
    }
    mTitleOfWindow[outP] = '\0';

    return string(mTitleOfWindow);
}

/**
 * This method returns a 40-char window title string.
 */
string
XHelper::getWindowTitleFromPID(const pid_t pid) {
    const vector<Window> windows = mXHelper->
        getWindowsStackedList();
    const int WINDOWS_SIZE = windows.size();

    for (int i = 0; i < WINDOWS_SIZE; i++) {
        const pid_t PID = getWindowPID(windows[i]);
        if (PID == pid) {
            return getWindowTitle(windows[i]);
        }
    }

    return "";
}

/**
 * This method determines which workspace a
 * window is visible on. result == -1 means all.
 */
long
XHelper::getWindowDesktop(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_DESKTOP", False), 0, 1, False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        type == XA_CARDINAL && properties) {
        const long result = *(long*) (void*) properties;
        XFree(properties);
        return result;
    }

    XFree(properties);
    return -1;
}

/**
 * This method sets the workspace value for a window.
 */
void
XHelper::setWindowDesktop(const Window window,
    const long workspace) {
    if (workspace == -1) {
        XChangeProperty(mDisplay, window, XInternAtom(mDisplay,
            "_NET_WM_DESKTOP", false), XA_ATOM, 32,
            PropModeReplace, NULL, 0);
        XFlush(mDisplay);
        return;
    }

    XChangeProperty(mDisplay, window, XInternAtom(mDisplay,
        "_NET_WM_DESKTOP", false), XA_CARDINAL, 32,
        PropModeReplace, (unsigned char*) &workspace, 1);
    XFlush(mDisplay);
}

/**
 * This method checks if a window is hidden.
 */
bool
XHelper::isWindowHidden(const Window window) {
    if (isWindowHiddenByNetWMState(window)) {
        return true;
    }
    if (isWindowHiddenByWMState(window)) {
        return true;
    }
    if (isDesktopShowing()) {
        return true;
    }

    return false;
}

/**
 * This method checks "_NET_WM_STATE" for
 * window HIDDEN attribute.
 */
bool
XHelper::isWindowHiddenByNetWMState(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32) {
        for (unsigned long i = 0; i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp("_NET_WM_STATE_HIDDEN",
                nameString) == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method checks "WM_STATE" for
 * window HIDDEN attribute.
 */
bool
XHelper::isWindowHiddenByWMState(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32 && nItems > 0 &&
        (*(long*) (void*) properties != NormalState)) {
        XFree(properties);
        return true;
    }
    XFree(properties);

    return false;
}

/**
 * This method checks if a window is sticky.
 */
bool
XHelper::isWindowSticky(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_STATE", False), 0, (~0L), False,
        AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        type == XA_ATOM) {
        for (unsigned long i = 0; i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp(nameString,
                "_NET_WM_STATE_STICKY") == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method checks if a window is a dock.
 */
bool
XHelper::isWindowDock(const Window window) {
    Atom type; int format;
    unsigned long nItems, unusedBytes;
    unsigned char* properties = nullptr;

    if (XGetWindowProperty(mDisplay, window, XInternAtom(
        mDisplay, "_NET_WM_WINDOW_TYPE", False), 0, (~0L),
        False, AnyPropertyType, &type, &format, &nItems,
        &unusedBytes, &properties) == Success &&
        format == 32) {
        for (int i = 0; (unsigned long)i < nItems; i++) {
            char* nameString = XGetAtomName(mDisplay,
                ((Atom*) (void*) properties) [i]);
            if (strcmp(nameString,
                "_NET_WM_WINDOW_TYPE_DOCK") == 0) {
                XFree(nameString);
                XFree(properties);
                return true;
            }
            XFree(nameString);
        }
    }
    XFree(properties);

    return false;
}

/**
 * This method scans the desktop windows list in
 * stacked order to determine if a requested window
 * is the one the mouse cursor is over.
 */
bool
XHelper::isWindowHovered(const Window window, const QPoint pos,
    const bool checkEntireWindow) {
    // Get info for the request.
    const long VISIBLE_WS = getVisibleDesktop();
    WinInfo* winInfo = getWinInfoForWindow(window);

    // Get candidate windows.
    vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // Define local cleanup functor.
    function<void(vector<WinInfo*>&)> uninitWinInfos =
        [](std::vector<WinInfo*>& winInfosVector) {
        for_each(winInfosVector.begin(), winInfosVector.end(),
            [](WinInfo* wininfoEach) {
                delete wininfoEach;
            }
        );
        winInfosVector.clear();
    };

    // Search candidates down from the top.
    for (int i = WININFO_SIZE - 1; i >= 0; i--) {
        // Hidden windows can't be hovered.
        const WinInfo* EACH = winInfos[i];
        if (EACH->isHidden) {
            continue;
        }

        // If the window is on a specific workspace & its not
        // the one we're displaying, we're not hovering it.
        if (VISIBLE_WS != -1 && EACH->onWorkspace != -1 &&
            VISIBLE_WS != EACH->onWorkspace) {
            continue;
        }

        // If the window doesn't contain the cursor x,y
        // position, we're not hovering it.
        if (!EACH->windowRect.contains(pos)) {
            continue;
        }

        // We found the hovered window, is it the requested one?
        if (EACH->window == window) {
            if (checkEntireWindow) {
                uninitWinInfos(winInfos);
                delete winInfo;
                return true;
            }
            const QRect CANVAS_RECT = QRect(
                EACH->windowRect.x() + mSettingsHelper->getCanvasXPos(),
                EACH->windowRect.y() + mSettingsHelper->getCanvasYPos(),
                mSettingsHelper->getCanvasWidth(),
                mSettingsHelper->getCanvasHeight());
            if (CANVAS_RECT.contains(pos)) {
                uninitWinInfos(winInfos);
                delete winInfo;
                return true;
            }
        }

        // Can't get EACH->window image, assume it traps events.
        XImage* windowImage = XGetImage(mDisplay, EACH->window,
            pos.x(), pos.y(), 1, 1, XAllPlanes(), ZPixmap);
        if (!windowImage) {
            uninitWinInfos(winInfos);
            delete winInfo;
            return false;
        } else {
            // Non transparent prevents hover of any window
            // underneath.
            if (XGetPixel(windowImage, 0, 0) != 0) {
                XDestroyImage(windowImage);
                uninitWinInfos(winInfos);
                delete winInfo;
                return false;
            }
            XDestroyImage(windowImage);
        }
    }

    // No windows hovered.
    uninitWinInfos(winInfos);
    delete winInfo;
    return false;
}

/**
 * Place window in stack order to be on top
 * of all other windows.
 */
void
XHelper::makeWindowStayOnTop(const Window window,
    const bool onOrOff) {
    const Atom MESSAGE = XInternAtom(mDisplay,
        "_NET_WM_STATE", False);

    const Atom NET_WM_STATE_ABOVE = XInternAtom(mDisplay,
        "_NET_WM_STATE_ABOVE", False);

    XEvent event{};
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = MESSAGE;

    event.xclient.format = 32;
    event.xclient.data.l[0] = onOrOff ? 1 : 0;
    event.xclient.data.l[1] = NET_WM_STATE_ABOVE;

    event.xclient.data.l[2] = 0; 
    event.xclient.data.l[3] = 1; 
    event.xclient.data.l[4] = 0;

    XSendEvent(mDisplay, DefaultRootWindow(mDisplay), False,
        SubstructureNotifyMask | SubstructureRedirectMask, &event);
    XFlush(mDisplay);
}

/**
 * Place window in stack order to be immediately
 * above desktop, yet below all other windows.
 */
void
XHelper::makeWindowStayOnBottom(const Window window,
    const bool onOrOff) {
    const Atom MESSAGE = XInternAtom(mDisplay,
        "_NET_WM_STATE", False);

    const Atom NET_WM_STATE_BELOW = XInternAtom(mDisplay,
        "_NET_WM_STATE_BELOW", False);

    XEvent event{};
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = MESSAGE;

    event.xclient.format = 32;
    event.xclient.data.l[0] = onOrOff ? 1 : 0;
    event.xclient.data.l[1] = NET_WM_STATE_BELOW;

    event.xclient.data.l[2] = 0; 
    event.xclient.data.l[3] = 1; 
    event.xclient.data.l[4] = 0;

    XSendEvent(mDisplay, DefaultRootWindow(mDisplay), False,
        SubstructureNotifyMask | SubstructureRedirectMask, &event);
    XFlush(mDisplay);
}

/**
 * Private initializer to create raw list of
 * currently active x11 windows.
 */
vector<WinInfo*>
XHelper::getWinInfoList() {
    const vector<Window> windows = getWindowsStackedList();
    const int WINDOWS_SIZE = windows.size();

    // 0 is desktop, add up.
    vector<WinInfo*> winInfos;
    for (int i = 0; i < WINDOWS_SIZE; i++) {
        winInfos.push_back(
            getWinInfoForWindow(windows[i])
        );
    }
    return winInfos;
}

/**
 * Getter to return WinInfo* for a Window.
 */
WinInfo*
XHelper::getWinInfoForWindow(const Window window) {
    WinInfo* winInfo = new WinInfo();
    winInfo->window = window;

    // Decorated Position.
    const QPoint WINDOW_POS = getWindowPosition(window);
    const QSize WINDOW_SIZE = getWindowSize(window);
    const QSize FRAME_OFFSET = getWindowFrameOffset(window);

    winInfo->windowRect = QRect(
        QPoint(WINDOW_POS.x() - FRAME_OFFSET.width(),
            WINDOW_POS.y() - FRAME_OFFSET.height()),
        QSize(WINDOW_SIZE.width() + FRAME_OFFSET.width(),
            WINDOW_SIZE.height() + FRAME_OFFSET.height())
    );

    // Set all other WinInfo fields.
    winInfo->onWorkspace = getWindowDesktop(winInfo->window);
    winInfo->mapState = getWindowMapstate(winInfo->window);

    winInfo->isSticky = (winInfo->onWorkspace == -1) ?
        true : isWindowSticky(winInfo->window);
    winInfo->isDock = isWindowDock(winInfo->window);
    winInfo->isHidden = isWindowHidden(winInfo->window);

    return winInfo;
}

/**
 * Helper to return Window as Hex string.
 */
string
XHelper::getWindowAsHexString(const Window window) {
    return string(format("0x0{:x}", (int) window));
}

/**
 * Helper to return PID as Hex string.
 */
string
XHelper::getPIDAsHexString(const pid_t pid) {
    return string(format("0x0{:x}", (int) pid));
}

/**
 * Debug method prints all WinInfo structs.
 */
void
XHelper::logAllWinInfoStructs() {
    XSync(mDisplay, false);

    vector<WinInfo*> winInfos = getWinInfoList();
    const int WININFO_SIZE = winInfos.size();

    // 0 is desktop, print down from top.
    logWinInfoStructColumns();
    for (int i = WININFO_SIZE - 1; i >= 0; i--) {
        logWinInfo(winInfos[i]);
        delete winInfos[i];
    }

    winInfos.clear();
}

/**
 * Debug method prints column headings for
 * WinInfo structs.
 */
void
XHelper::logWinInfoStructColumns() {
    cout << endl << XCOLOR_GREEN <<
        "---window---  Titlebar Name"
        "                             WS  Map "
        "---Position--  ----Size-----  "
        "Attributes" <<
        XCOLOR_NORMAL << endl;
}

/**
 * Debug method prints a requested windows
 * WinInfo struct.
 */
void
XHelper::logWinInfo(const WinInfo* winInfo) {
    printf("[0x%08lx]  %s  %2i  %2i  %5i , %-5i  %5i "
        ", %-5i  %s%s%s\n",
        winInfo->window, getWindowTitle(winInfo->window).c_str(),
        winInfo->onWorkspace, winInfo->mapState,
        (int) winInfo->windowRect.x(),
        (int) winInfo->windowRect.y(),
        (int) winInfo->windowRect.width(),
        (int) winInfo->windowRect.height(),
        winInfo->isDock ? "dock " : "",
        winInfo->isSticky ? "sticky " : "",
        winInfo->isHidden ? "hidden" : "");
}

/**
 * This method drains & debugs pending display events.
 */
void
XHelper::debugX11EventQueue() {
    XEvent event;

    XFlush(mDisplay);
    while (XPending(mDisplay)) {

        XNextEvent(mDisplay, &event);
        const XAnyEvent* EVENT = (XAnyEvent*) &event;
        debugXAnyEvent(EVENT);
    }
}
/**
 * Helper method to debug XAnyEvent.
 */
void
XHelper::debugXAnyEvent(const XAnyEvent* event) {
    const string THIS_SERIAL = string(format("{:09d}",
        event->serial));
    mEventSerialString = THIS_SERIAL == mPrevEventSerialString ?
        "     " : THIS_SERIAL;
    mPrevEventSerialString = THIS_SERIAL;

    // Type = 3; KeyRelease.
    if (event->type == KeyRelease) {
        debugXKeyEvent((const XKeyEvent*) event);
        return;
    }

    // Type = 12; Expose.
    if (event->type == Expose) {
        debugXExposeEvent((const XExposeEvent*) event);
        return;
    }

    // Type = 17; DestroyNotify.
    if (event->type == DestroyNotify) {
        debugXDestroyWindowEvent((const XDestroyWindowEvent*)
            event);
        return;
    }

    // Type = 18; UnmapNotify.
    if (event->type == UnmapNotify) {
        debugXUnmapEvent((const XUnmapEvent*) event);
        return;
    }

    // Type = 19; MapNotify.
    if (event->type == MapNotify) {
        debugXMapEvent((const XMapEvent*) event);
        return;
    }

    // Type = 21; ReparentNotify.
    if (event->type == ReparentNotify) {
        debugXReparentEvent((const XReparentEvent*) event);
        return;
    }

    // Type = 22; ConfigureNotify.
    if (event->type == ConfigureNotify) {
        debugXConfigureEvent((const XConfigureEvent*) event);
        return;
    }

    // Type = 28; PropertyNotify.
    if (event->type == PropertyNotify) {
        debugXPropertyEvent((const XPropertyEvent*) event);
        return;
    }

    // Type = 33; ClientMessage.
    if (event->type == ClientMessage) {
        debugXClientMessageEvent((const XClientMessageEvent*)
            event);
        return;
    }

    // Type = 6; MotionNotify, ignore - too many.
    if (event->type == MotionNotify) {
        return;
    }

    printf("XAnyEvent            W : 0x%08lx"
        " %9s  %05d"
        "  Type: %02d                    "
        "                  ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event, event->type,
        XPending(event->display));
}

/**
 * Helper method to debug XKeyEvent.
 */
void
XHelper::debugXKeyEvent(const XKeyEvent* event) {
    printf("XKeyEvent            W : 0x%08lx"
        " %9s  %05d"
        "  @ %05d x %05d"
        "                               ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        event->x, event->y,
        XPending(event->display));
}

/**
 * Helper method to debug XExposeEvent.
 */
void
XHelper::debugXExposeEvent(const XExposeEvent* event) {
    printf("XExposeEvent         W : 0x%08lx"
        " %9s  %05d"
        "  %05d x %05d - %05d x %05d  count: %03d     ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        event->x, event->y, event->width, event->height,
        event->count, XPending(event->display));
}

/**
 * Helper method to debug XDestroyWindowEvent.
 */
void
XHelper::debugXDestroyWindowEvent(const XDestroyWindowEvent*
    event) {
    printf("XDestroyWindowEvent  W : 0x%08lx"
        " %9s  %05d"
        "                                                ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        XPending(event->display));
}

/**
 * Helper method to debug XUnmapEvent.
 */
void
XHelper::debugXUnmapEvent(const XUnmapEvent* event) {
    printf("XUnmapEvent          W : 0x%08lx"
        " %9s  %05d"
        "                                                ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        XPending(event->display));
}

/**
 * Helper method to debug XMapEvent.
 */
void
XHelper::debugXMapEvent(const XMapEvent* event) {
    printf("XMapEvent            W : 0x%08lx"
        " %9s  %05d"
        "                                                ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        XPending(event->display));
}

/**
 * Helper method to debug XReparentEvent.
 */
void
XHelper::debugXReparentEvent(const XReparentEvent* event) {
    printf("XReparentEvent       W : 0x%08lx"
        " %9s  %05d"
        "  %05d x %05d"
        "                                 ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        event->x, event->y,
        XPending(event->display));
}

/**
 * Helper method to debug XConfigureEvent.
 */
void
XHelper::debugXConfigureEvent(const XConfigureEvent* event) {
    int posX; int posY; Window unused;
    XTranslateCoordinates(mDisplay, event->window,
        RootWindow(mDisplay, DefaultScreen(mDisplay)),
        0, 0, &posX, &posY, &unused);

    printf("XConfigureEvent      W : 0x%08lx"
        " %9s  %05d"
        "  %05d x %05d - %05d x %05d"
        "                 ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        posX, posY, event->width, event->height,
        XPending(event->display));
}

/**
 * Helper method to debug XPropertyEvent.
 */
void
XHelper::debugXPropertyEvent(const XPropertyEvent* event) {
    printf("XPropertyEvent       W : 0x%08lx"
        " %9s  %05d"
        "  [ %-35s ]"
        "       ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        XGetAtomName(event->display, event->atom),
        XPending(event->display));
}

/**
 * Helper method to debug XClientMessageEvent.
 */
void
XHelper::debugXClientMessageEvent(const XClientMessageEvent*
    event) {
    printf("XClientMessageEvent  W : 0x%08lx"
        " %9s  %05d"
        "                                                ... %d.\n",
        event->window,
        mEventSerialString.c_str(), event->send_event,
        XPending(event->display));
}
