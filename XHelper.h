
#pragma once

/**
 * XHelper provides common X related methods.
 */
class XHelper {

    typedef int MapState;

    public:
        /**
         * Constructor.
         */
        XHelper();

        /**
         * This method traps and handles X11 errors.
         */
        static int handleX11ErrorEvent(Display* display,
            XErrorEvent* event);

        /**
         * This method returns the Window Managers name.
         */
        string getWindowManagerName();

        /**
         * Helper method to check if the DM can report
         * information about the WM.
         */
        bool canDisplayReportWMName();

        /**
         * Helper method to get the Root Window of the DM.
         */
        Window getRootWindowFromDisplay();

        /**
         * Helper method to get the WM Name from the
         * DM's Root Window.
         */
        string getWMNameFromRootWindow(const Window rootWindow);

        /**
         * This method determines if a compositor is running.
         */
        bool isACompositorRunning();

        /**
         * This method determines the Compositors name.
         */
        QString getCompositorName();

        /**
         * Check if the display supports transparent pointer events.
         */
        bool isTransparentToPointer();

        /**
         * Check if display supports TrueColor32 visual transparency.
         */
        bool isTransparentVisually();

        /**
         * This method checks if the desktop is currently
         * being shown, (which hides all windows).
         */
        bool isDesktopShowing();

        /**
         * This method returns the number of the current workspace,
         * where the OS allows multiple / virtual workspaces.
         *
         * Result == -1 means one big workspace is visible (Viewport).
         */
        long getVisibleDesktop();

        /**
         * This method changes the users visible desktop.
         */
        void setVisibleDesktop(const long desktop);

        /**
         * This method returns the maximum number of allowable
         * workspaces, where the OS allows multiple / virtual workspaces.
         *
         * Result == -1 means one big workspace is visible (Viewport).
         */
        long getMaximumDesktops();

        /**
         * Method returns a a list of active X11 windows
         * in stacking order.
         */
        vector<Window> getWindowsStackedList();

        /**
         * Method returns if a window is in the list
         * of active X11 windows.
         */
        bool isWindowInStackedList(const Window window);

        /**
         * This method waits until a window is in the list
         * of active X11 windows.
         */
        bool waitForWindowInStackedList(const Window window,
            const int maxWaitTimeMS);

        /**
         * This method waits until a window is in the list of
         * active X11 windows and mapped or unmapped as requested.
         */
        bool waitForWindowMapState(const Window window,
            const MapState mapState, const int maxWaitTimeMS);

        /**
         * This method waits until a window is in the list of
         * active X11 windows and @ requested position.
         */
        bool waitForWindowMove(const Window window,
            const QPoint position, const int maxWaitTimeMS);

        /**
         * Getter to return WinInfo* for a Window.
         */
        int getWindowStackNumber(const Window window);

        /**
         * Getter for Window Position.
         */
        QPoint getWindowPosition(const Window window);

        /**
         * Getter for Window Colormap.
         */
        QSize getWindowSize(const Window window);

        /**
         * Getter for Decorated Window Size.
         */
        QSize getWindowFrameOffset(const Window window);

        /**
         * Getter for Window Mapstate.
         */
        int getWindowMapstate(const Window window);

        /**
         * Gets window PID.
         */
        pid_t getWindowPID(const Window window);

        /**
         * Sets window PID.
         */
        void setWindowPID(const Window window);

        /**
         * Sets window titlebar, border to desired visibility.
         */
        void setWindowType(const Window window,
            const Atom windowType);

        /**
         * This method returns a 40-char window title string.
         */
        string getWindowTitle(const Window window);

        /**
         * This method returns a 40-char window title string.
         */
        string getWindowTitleFromPID(const pid_t pid);

        /**
         * This method determines which workspace a
         * window is visible on. result == -1 means all.
         */
        long getWindowDesktop(const Window window);

        /**
         * This method sets the workspace value for a window.
         */
        void setWindowDesktop(const Window window,
            const long workspace);

        /**
         * This method checks if a window is hidden.
         */
        bool isWindowHidden(const Window window);

        /**
         * This method checks "_NET_WM_STATE" for
         * window HIDDEN attribute.
         */
        bool isWindowHiddenByNetWMState(const Window window);

        /**
         * This method checks "WM_STATE" for
         * window HIDDEN attribute.
         */
        bool isWindowHiddenByWMState(const Window window);

        /**
         * This method checks if a window is sticky.
         */
        bool isWindowSticky(const Window window);

        /**
         * This method checks if a window is a dock.
         */
        bool isWindowDock(const Window window);

        /**
         * This method scans the desktop windows list in
         * stacked order to determine if a requested window
         * is the one the mouse cursor is over.
         */
        bool isWindowHovered(const Window window,
            const QPoint pos, const bool checkEntireWindow);

        /**
         * Place window in stack order to be on top
         * of all other windows.
         */
        void makeWindowStayOnTop(const Window window,
            const bool onOrOff);

        /**
         * Place window in stack order to be immediately
         * above desktop, yet below all other windows.
         */
        void makeWindowStayOnBottom(const Window window,
            const bool onOrOff);

        /**
         * Private initializer to create raw list of
         * currently active x11 windows.
         */
        vector<WinInfo*> getWinInfoList();

        /**
         * Getter to return WinInfo* for a Window.
         */
        WinInfo* getWinInfoForWindow(const Window window);

        /**
         * Helper to return Window as Hex string.
         */
        string getWindowAsHexString(const Window window);

        /**
         * Helper to return PID as Hex string.
         */
        string getPIDAsHexString(const pid_t pid);

        /**
         * Debug method prints all WinInfo structs.
         */
        void logAllWinInfoStructs();

        /**
         * Debug method prints column headings for
         * WinInfo structs.
         */
        void logWinInfoStructColumns();

        /**
         * Debug method prints a requested windows
         * WinInfo struct.
         */
        void logWinInfo(const WinInfo* winInfo);

        /**
         * This method drains & debugs pending display events.
         */
        void debugX11EventQueue();

        /**
         * Helper method to debug XAnyEvent.
         */
        void debugXAnyEvent(const XAnyEvent* event);

        /**
         * Helper methods to debug XEvents.
         */
        void debugXKeyEvent(const XKeyEvent* event);
        void debugXExposeEvent(const XExposeEvent* event);
        void debugXDestroyWindowEvent(const XDestroyWindowEvent*
            event);
        void debugXUnmapEvent(const XUnmapEvent* event);
        void debugXMapEvent(const XMapEvent* event);
        void debugXReparentEvent(const XReparentEvent* event);
        void debugXConfigureEvent(const XConfigureEvent* event);
        void debugXPropertyEvent(const XPropertyEvent* event);
        void debugXClientMessageEvent(const XClientMessageEvent*
            event);

    private:
        // Members.
        string mPrevEventSerialString = "";
        string mEventSerialString = "         ";

        Atom mAtomDMSupportsWMCheck{};
        Atom mAtomGetWMName{};
        Atom mAtomGetUTF8String{};
};
