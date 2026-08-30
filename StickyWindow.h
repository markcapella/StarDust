
#pragma once

/**
 * StickyWindow class wraps an x11 Window object.
 *
 */
class StickyWindow {

    public:
        static inline constexpr QPoint INVALID_POSITION { -1, -1 };

        static inline const long OBSERVABLE_EVENTS =
            ConfigureNotify | StructureNotifyMask | PropertyChangeMask |
            EnterWindowMask | LeaveWindowMask |
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
            ExposureMask;

        static inline constexpr chrono::milliseconds
            CURSOR_WATCHER_DELAY_MS{5};
        static inline constexpr chrono::milliseconds
            CLICK_LEN_MS{25};

        // Constructor.
        StickyWindow();
        ~StickyWindow();

        /**
         * Overriden show() method ensures we stay on bottom
         * when window is "stuck", window border state != visible.
         */
        void show();

        /**
         * Overriden draw() method ensures we have a transparent
         * window with PinButton visible on mouse hover.
         */
        void draw();

        /**
         * Erase window method.
         */
        void eraseWindow();

        /**
         * Overriden hide() method.
         */
        void hide();

        /**
         * Overriden resize() method ensures we remember window
         * position & size on restarts.
         *
         * Adjusts Control Button positions & their Input
         * Shape Regions to the new locations.
         */
        void resize(const int xPos, const int yPos,
            const int width, const int height);

        /**
         * Main X11 event cycle Handler.
         */
        void run();

        /**
         * This method returns StickyWindows internal x11 window.
         */
        Window getX11Window() const {
            return mX11Window;
        }

    private:
        /**
         * Initialize Transparency & TrueColor 32.
         */
        bool initVisualTransparency();

        /**
         * This method sets StickyWindows internal x11 window.
         */
        void setX11Window(const Window window);

        /**
         * Create a new X11 window with appropriate decorations.
         */
        Window createX11Window();

        /**
         * Determine centered position on screen for first
         * time widget is run.
         */
        void defineWindowOnFirstRun();

        /**
         * Set window type as Dock. Awesome WM uses
         * _NET_WM_WINDOW_TYPE_SPLASH.
         */
        void setStickyWindowType();

        /**
         * Set window to stay on bottom or float as normal
         * based on configState.
         */
        void setWindowStickPosition();

        /**
         * Callback method for Autohide controls timer.
         */
        void setConfigModeOff();

        /**
         * This method defines the Control buttons.
         */
        void createAllWindowButtons();

        /**
         * This method updates the defined Control buttons.
         */
        void updateAllWindowButtons();

        /**
         * Draw all visible buttons.
         */
        void drawAllWindowButtons();

        /**
         * Cursor watcher makes visible any Pin button or corner
         * Control button visible the mouse is hovering.
         */
        void setAllControlsVisibility(const bool optimize);

        /**
         * Set visibility state of the four corner control buttons on
         * or off based on ConfigMode and update auto hide timer.
         */
        void setControlButtonsVisibility();

        /**
         * Setter for Hovered PinButton visibility state.
         */
        void setHoveredPinButtonVisibility(const bool visibility);

        /**
         * Setter for all other Hovered ControlButton visibility state.
         */
        void setHoveredControlButtonVisibility(const QPoint position);

        /**
         * Press hovered button, & return it's position.
         */
        QPoint pressHoveredButton(const QPoint position);

        /**
         * Return draggable status of pressed button.
         */
        bool isPressedButtonDraggable();

        /**
         * Return sizeable status of pressed button.
         */
        bool isPressedButtonSizable();

        /**
         * Clicking pressed hovered, doesn't trigger click if we
         * press button, drag off, then release.
         */
        void clickPressedHoveredButton(const QPoint position);

        /**
         * Toggle config mode.
         */
        void clickPressedPinButton();

        /**
         * Unpress all UI Buttons.
         */
        void unPressAllWindowButtons();

        /**
         * Define Canvas position inside Window.
         */
        void defineWindowCanvasPosition();

        /**
         * Define Canvas size inside Window.
         */
        void defineWindowCanvasSize();

        /**
         * This method updates the clocks time string for draw().
         */
        void updateWindowCanvas();

        /**
         * Main X11 Event handler.
         */
        bool handleX11EventQueue();

        /**
         * Ensure window opens on valid remembered desktop.
         */
        void rangeCheckPreferredDesktopSetting();

        /**
         * Update the Config Dialog if it's active &
         * the UI needs updating.
         */
        void updateActiveConfigDialog();

        /**
         * Receives an event from Qt ConfigDialog that it has
         * completed with new user config settings.
         */
        void receiveConfigDialogUpdatedEvent(
            const bool canvasNeedsRedraw);

        /**
         * Perform window drag.
         */
        void dragWindowToPoint(const QPoint position);

        /**
         * Perform window resizing.
         */
        void resizeWindowToPoint(const QPoint position);

        /**
         * Determine if the current StickyWindow overhanges the screen
         * (current desktop) edges and moves it entirely into window
         * according to user pref.
         */
        void maybeAdjustWindowOverhang();

        /**
         * Members.
         */
        Window mX11Window = None;
        char* mWindowTitle = nullptr;
        Picture mRenderPicture{};

        bool mIsVisuallyTransparent = false;
        XVisualInfo mVisualInfoStruct { };
        Colormap mColorMap { };

        vector<Button*> mButtons;
        mutable recursive_mutex mButtonsMutLock;
        unique_ptr<QTimer> mAutoHideControlsTimer{nullptr};

        PinButton* mPinButton = nullptr;
        QuitButton* mQuitButton = nullptr;
        ConfigButton* mConfigButton = nullptr;
        MoveButton* mMoveButton = nullptr;
        SizeButton* mSizeButton = nullptr;

        // handleX11EventQueue.
        Window mTranslateWindow = None;
        int mTranslatePosX = -1;
        int mTranslatePosY = -1;

        // ButtonPress.
        QSize mClickedWindowSize;
        QPoint mClickedWindowPosition;
        QPoint mClickedButtonPosition;

        QSize mUnClickedWindowSize;
        QPoint mUnClickedWindowPosition;

        QPoint mDragMoveButtonOffset{};
        QPoint mDragResizeButtonOffset{};

        bool mIsMouseClicked = false;
        bool mIsSizingWindow = false;
        bool mIsMovingWindow = false;

        int mPreviousDesktop = -1;

        // Optimization to avoid needless cursor raycasts.
        QPoint mCursorPrevHoverPosition{ -1, -1 };
        QPoint mCursorHoverPosition{};

};
