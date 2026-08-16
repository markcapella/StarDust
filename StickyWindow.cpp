
#include "Global.h"

/**
 * StickyWindow class wraps an x11 Window object.
 */
StickyWindow::StickyWindow() {
    // Init visual transparency & TrueColor 32.
    mIsVisuallyTransparent = initVisualTransparency();
    if (!mIsVisuallyTransparent) {
        QMessageBox::information(NULL, APP_NAME, "Visual transparency "
            "unavailable with this Desktop, FATAL.");
        cout << endl << XCOLOR_RED << "Visual transparency unavailable "
            "with this Desktop, FATAL." << XCOLOR_NORMAL << endl;
        return;
    }

    // Create & set base x11 window.
    setX11Window(createX11Window());

    // Create control buttons.
    createAllWindowButtons();

    // Instantiate widget canvas.
    mCanvas = new Canvas(getX11Window(), mButtons);
    setControlButtonsVisibility();

    // Create autohide timer for control buttons,
    // then set control buttons visibility.
    mAutoHideControlsTimer = make_unique<QTimer>();
    mAutoHideControlsTimer->setSingleShot(true);
    QObject::connect(mAutoHideControlsTimer.get(), &QTimer::timeout,
        [this] () {
        setConfigModeOff();
    });
}

/**
 * StickyWindow destructor, cleanup Window object.
 */
StickyWindow::~StickyWindow() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    mCanvas->uninitCanvas();
    delete mCanvas;

    const int BUTTONS_SIZE = mButtons.size();
    for (int i = 0; i < BUTTONS_SIZE; i++) {
        delete mButtons[i];
    }
    mButtons.clear();

    // Uninit all.
    if (mIsVisuallyTransparent) {
        XFreeColormap(mDisplay, mColorMap);
    }

    if (mX11Window != None) {
        XUnmapWindow(mDisplay, mX11Window);
        XDestroyWindow(mDisplay, mX11Window);
        mX11Window = None;
    }
}

/**
 * Overriden show() method ensures we stay on bottom
 * when window is "stuck", window border state != visible.
 */
void
StickyWindow::show() {
    XMapWindow(mDisplay, mX11Window);
}

/**
 * Overriden draw() method ensures we have a transparent
 * window with PinButton visible on mouse hover.
 */
void
StickyWindow::draw() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(mDisplay, mVisualInfoStruct.visual);
    Picture renderPic = XRenderCreatePicture(mDisplay, mX11Window,
        RENDER_FORMAT, 0, nullptr);

    // If drawing on wrong desktop, erase instead.
    const int VISIBLE_DESKTOP = mXHelper->getVisibleDesktop();
    const int PREFERRED_DESKTOP = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP);
    if (VISIBLE_DESKTOP != -1 && PREFERRED_DESKTOP != -1 &&
        (VISIBLE_DESKTOP != PREFERRED_DESKTOP)) {
        mCanvas->setCanvasHidden(); // Not on this desktop.
        eraseWindow();
        return;
    }

    // Draw rubberband instead of canvas while resizing.
    if (mIsSizingWindow) {
        mCanvas->setCanvasHidden(); // By resize rubberband area.
        const int RUBBERBAND_OPACITY = 255;
        const XRenderColor RUBBERBAND_COLOR = newRenderColor(
            BLACK_RCOLOR.red, BLACK_RCOLOR.green,
            BLACK_RCOLOR.blue, RUBBERBAND_OPACITY);

        const int RUBBERBAND_BACKGROUND_OPACITY = 128;
        const XRenderColor RUBBERBAND_BACKGROUND_COLOR = newRenderColor(
            RUBBERBAND_RCOLOR.red, RUBBERBAND_RCOLOR.green,
            RUBBERBAND_RCOLOR.blue, RUBBERBAND_BACKGROUND_OPACITY);

        XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
            &RUBBERBAND_COLOR, 0, 0, mSettingsHelper->
            getWindowWidth(), mSettingsHelper->getWindowHeight());
        XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
            &RUBBERBAND_BACKGROUND_COLOR, 1, 1, mSettingsHelper->
            getWindowWidth() - 2, mSettingsHelper->getWindowHeight() - 2);
    } else {
        // Else, draw Canvas.
        mCanvas->setCanvasVisible();
        mCanvas->drawCanvas();
        drawAllWindowButtons();
    }

    XRenderFreePicture(mDisplay, renderPic);
    XFlush(mDisplay);
}

/**
 * Erase window method.
 */
void
StickyWindow::eraseWindow() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(mDisplay, mVisualInfoStruct.visual);
    Picture renderPic = XRenderCreatePicture(mDisplay, mX11Window,
        RENDER_FORMAT, 0, nullptr);

    XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
        &TRANSPARENT_RCOLOR, 0, 0, mSettingsHelper->
        getWindowWidth(), mSettingsHelper->getWindowHeight());

    // Cleanup & done.
    XRenderFreePicture(mDisplay, renderPic);
    XFlush(mDisplay);
}

/**
 * Overriden hide() method.
 */
void
StickyWindow::hide() {
}

/**
 * Overriden resize() method ensures we remember window
 * position & size on restarts.
 *
 * Adjusts clickable button positions & their Input
 * Shape Regions to the new locations.
 */
void
StickyWindow::resize(const int xPos, const int yPos,
    const int width, const int height) {

    mSettingsHelper->setWindowXPos(xPos);
    mSettingsHelper->setWindowYPos(yPos);
    mSettingsHelper->setWindowWidth(width);
    mSettingsHelper->setWindowHeight(height);

    // Reset position hints on resize.
    XSizeHints* hints = XAllocSizeHints();
    if (hints) {
        hints->flags = USPosition | PMinSize;
        hints->x = xPos;
        hints->y = yPos;
        hints->min_width = (int)
            mSettingsHelper->getWindowMinimumWidth();
        hints->min_height = (int)
            mSettingsHelper->getWindowMinimumHeight();
        XSetWMNormalHints(mDisplay, mX11Window, hints);
        XFree(hints);
    }

    draw();
}

/**
 * Main X11 event cycle Handler.
 */
void
StickyWindow::run() {
    while (true) {
        // Process xEvents, close when requested.
        if (handleX11EventQueue()) {
            mCanvas->uninitCanvas();
            break;
        }

        // Custom hover logic for controls.
        setAllControlsVisibility();

        // Support ConfigDialog loop.
        QCoreApplication::processEvents();
    }
}

/**
 * Initialize Transparency & TrueColor 32.
 */
bool
StickyWindow::initVisualTransparency() {
    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        return false;
    }

    const int VISUAL_COLOR_DEPTH = 32;
    if (XMatchVisualInfo(mDisplay, DefaultScreen(mDisplay),
        VISUAL_COLOR_DEPTH, TrueColor, &mVisualInfoStruct)) {
        mColorMap = XCreateColormap(mDisplay, RootWindow(mDisplay,
            DefaultScreen(mDisplay)), mVisualInfoStruct.visual,
                AllocNone);
        return true;
    }

    return false;
}

/**
 * Private method sets StickyWindows internal x11 window.
 */
void 
StickyWindow::setX11Window(const Window window) {
    mX11Window = window;
}

/**
 * Create a new X11 window with appropriate decorations.
 */
Window
StickyWindow::createX11Window() {
    // Determine if Widget first time run.
    const bool INITIAL_RUN = QPoint(mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos()) == INVALID_POINT;

    // Initial setup for Widget first time run.
    if (INITIAL_RUN) {
        defineWindowOnFirstRun();
        defineWindowCanvasPosition();
    }

    // Create window @ position & size.
    XSetWindowAttributes wAttrs;
    wAttrs.colormap = mColorMap;
    wAttrs.border_pixel = 0;
    wAttrs.event_mask = OBSERVABLE_EVENTS;

    mX11Window = XCreateWindow(mDisplay, RootWindow(mDisplay,
        mVisualInfoStruct.screen), mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos(), mSettingsHelper->
        getWindowWidth(), mSettingsHelper->getWindowHeight(), 1,
        mVisualInfoStruct.depth, InputOutput, mVisualInfoStruct.visual,
        CWColormap | CWEventMask | CWBorderPixel, &wAttrs);

    if (mX11Window == None) {
        cout << endl << XCOLOR_RED << "X11 failed to create a window - "
            "Fatal." << XCOLOR_NORMAL << endl << endl;
        return mX11Window;
    }

    // Filter events we care to see in event loop.
    XSelectInput(mDisplay, mX11Window, OBSERVABLE_EVENTS);
    XSelectInput(mDisplay, DefaultRootWindow(mDisplay),
        PropertyChangeMask);
    XSetWMProtocols(mDisplay, mX11Window, &mCloseAppMessage, 1);

    // Set procID on our window, for RecentsHelper().
    mXHelper->setWindowPID(mX11Window);

    // Set window Size Hints, position & minimum size.
    XSizeHints* hints = XAllocSizeHints();
    if (hints) {
        hints->flags = USPosition | PMinSize;
        hints->x = mSettingsHelper->getWindowXPos();
        hints->y = mSettingsHelper->getWindowYPos();
        hints->min_width = (int) mSettingsHelper->
            getWindowMinimumWidth();
        hints->min_height = (int) mSettingsHelper->
            getWindowMinimumHeight();
        XSetWMNormalHints(mDisplay, mX11Window, hints);
        XFree(hints);
    }

    // Set the window title.
    const QString FIRST_RECENTS_NAME =
        mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME =
        mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + APP_RECENT_NAME;
    }
    mWindowTitle = strdup(TITLE.toUtf8().constData());
    XSetStandardProperties(mDisplay, mX11Window, mWindowTitle,
        mWindowTitle, None, nullptr, 0, nullptr);
    free(mWindowTitle);

    // Ensure we're placed on all desktops. We appear to be
    // "on" or "off" a desktop through logical show/hides.
    mXHelper->setWindowDesktop(mX11Window, -1);

    // Ensure window opens on valid remembered desktop.
    rangeCheckPreferredDesktopSetting();

    // Set "StickyWindow" type, show window, set config state.
    setStickyWindowType();
    show();
    setWindowStickPosition();

    // Apply strict configuration to the window. Awesome WM
    // specifically needs this.
    XWindowChanges changes;
    changes.x = mSettingsHelper->getWindowXPos();
    changes.y = mSettingsHelper->getWindowYPos();
    changes.width = mSettingsHelper->getWindowWidth();
    changes.height = mSettingsHelper->getWindowHeight();
    XConfigureWindow(mDisplay, mX11Window,
        CWX | CWY | CWWidth | CWHeight, &changes);
    XFlush(mDisplay);

    // Done!
    return mX11Window;
}

/**
 * Determine centered position & size for first run.
 */
void
StickyWindow::defineWindowOnFirstRun() {
    const int SCREEN_WIDTH = WidthOfScreen(
        DefaultScreenOfDisplay(mDisplay));
    const int SCREEN_HEIGHT = HeightOfScreen(
        DefaultScreenOfDisplay(mDisplay));

    mSettingsHelper->setWindowWidth(mSettingsHelper->getCanvasWidth());
    mSettingsHelper->setWindowHeight(mSettingsHelper->getCanvasHeight());

    mSettingsHelper->setWindowXPos((SCREEN_WIDTH -
        mSettingsHelper->getWindowWidth()) / 2);
    mSettingsHelper->setWindowYPos((SCREEN_HEIGHT -
        mSettingsHelper->getWindowHeight()) / 2);
}

/**
 * Set window type as Dock. Awesome WM uses
 * _NET_WM_WINDOW_TYPE_SPLASH.
 */
void
StickyWindow::setStickyWindowType() {
    // Just for Awesome WM.
    const QString THIS_WM = mXHelper->getWindowManagerName().c_str();
    const QString AWESOME_WM = "awesome";

    if (THIS_WM == AWESOME_WM) {
        const Atom STICKY_WINDOW_TYPE = XInternAtom(mDisplay,
            "_NET_WM_WINDOW_TYPE_SPLASH", false);
        mXHelper->setWindowType(mX11Window, STICKY_WINDOW_TYPE);
        return;
    }

    // The default.
    const Atom STICKY_WINDOW_TYPE = XInternAtom(mDisplay,
        "_NET_WM_WINDOW_TYPE_DOCK", false);
    mXHelper->setWindowType(mX11Window, STICKY_WINDOW_TYPE);
}

/**
 * Set window to stay on bottom or float as normal
 * based on configState.
 */
void
StickyWindow::setWindowStickPosition() {
    const bool PREFER_ONTOP = mSettingsHelper->getBoolSetting(
        SettingsHelper::ON_TOP_INSTEAD);

    // Ensure below is explicitly turned off before enabling above.
    if (PREFER_ONTOP) {
        mXHelper->makeWindowStayOnBottom(mX11Window, false);
        mXHelper->makeWindowStayOnTop(mX11Window, true);
    } else {
        // Ensure above is explicitly turned off before enabling below.
        mXHelper->makeWindowStayOnTop(mX11Window, false);
        mXHelper->makeWindowStayOnBottom(mX11Window, true);
    }
}

/**
 * Callback method for Autohide controls timer.
 */
void
StickyWindow::setConfigModeOff() {
    // If visibility already off, we're done.
    const bool CONFIG_MODE = mSettingsHelper->getConfigMode();
    if (!CONFIG_MODE) {
        return;
    }

    // Set visibility off, & done.
    mSettingsHelper->setConfigMode(false);
    setControlButtonsVisibility();
}

/**
 * This method defines the Control buttons.
 */
void
StickyWindow::createAllWindowButtons() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    mPinButton = new PinButton(mSettingsHelper->
        getWindowWidth() - 2 * Button::BUTTON_WIDTH, 0);

    mQuitButton = new QuitButton(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH, 0);
    mConfigButton = new ConfigButton(0, mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);
    mMoveButton = new MoveButton(0, 0);

    mSizeButton = new SizeButton(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH, mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);

    // Pin Button First.
    mButtons.push_back(mPinButton);

    // All others.
    mButtons.push_back(mQuitButton);
    mButtons.push_back(mConfigButton);
    mButtons.push_back(mMoveButton);
    mButtons.push_back(mSizeButton);
}

/**
 * This method updates the defined Control buttons.
 */
void
StickyWindow::updateAllWindowButtons() {
    mPinButton->setX(mSettingsHelper->
        getWindowWidth() - 2 * Button::BUTTON_WIDTH);
    mPinButton->setY(0);

    mQuitButton->setX(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH);
    mQuitButton->setY(0);

    mConfigButton->setX(0);
    mConfigButton->setY(mSettingsHelper->getWindowHeight() -
        Button::BUTTON_HEIGHT);

    mMoveButton->setX(0);
    mMoveButton->setY(0);

    mSizeButton->setX(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH);
    mSizeButton->setY(mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);
}

/**
 * Draw all visible buttons & set underlying input shapes.
 */
void
StickyWindow::drawAllWindowButtons() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    vector<XRectangle> rects;
    const int BUTTONS_END = mButtons.size();
    for (int i = 0; i < BUTTONS_END; i++) {
        if (mButtons[i]->isVisible()) {
            mButtons[i]->draw(getX11Window());

            XRectangle buttonInputRegion {
                .x = (short) mButtons[i]->getX(),
                .y = (short) mButtons[i]->getY(),
                .width = (unsigned short) mButtons[i]->getWidth(),
                .height = (unsigned short) mButtons[i]->getHeight()
            };
            rects.push_back(buttonInputRegion);
        }
    }

    // Explicitly set an empty input.
    if (!rects.empty()) {
        XShapeCombineRectangles(mDisplay, mX11Window, ShapeInput,
            0, 0, rects.data(), rects.size(), ShapeSet, Unsorted);
    } else {
        XRectangle emptyRect = {0, 0, 0, 0};
        XShapeCombineRectangles(mDisplay, mX11Window, ShapeInput,
            0, 0, &emptyRect, 0, ShapeSet, Unsorted);
    }

    XFlush(mDisplay);
}

/**
 * All screen cursor watcher to set hovered control buttons
 * visibility during hover.
 */
void
StickyWindow::setAllControlsVisibility() {
    // Find the cursor location relative to the root window.
    Window rootWindow = None;
    int rootX = -1;
    int rootY = -1;
    Window window = None;
    int winX = -1;
    int winY = -1;
    unsigned int clickStatus = 0;

    if (!XQueryPointer(mDisplay, DefaultRootWindow(mDisplay),
        &rootWindow, &window, &rootX, &rootY, &winX, &winY,
        &clickStatus)) {
        cout << XCOLOR_YELLOW << "Failed to query Cursor location." <<
            XCOLOR_NORMAL << endl;
        return;
    }

    // Early out if not hovered anywhere.
    const QPoint CURSOR_ROOT_POS = QPoint(rootX, rootY);
    const QRect WINDOW_RECT = QRect(mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos(), mSettingsHelper->getWindowWidth(),
        mSettingsHelper->getWindowHeight());
    const bool IS_WINDOW_HOVERED = mXHelper->isWindowHoveredAtPos(
        getX11Window(), WINDOW_RECT, CURSOR_ROOT_POS);

    if (!IS_WINDOW_HOVERED) {
        setHoveredPinButtonVisibility(false);
        setHoveredControlButtonVisibility(QPoint(-1, -1));
        return;
    }

    // Set visibility for Pin Button control. It's visible on
    // window hovered and pinButtonEnabled or pinButton hovered.
    const QRect PIN_BUTTON_RECT = QRect(
        mSettingsHelper->getWindowXPos() + mButtons[0]->getX(),
        mSettingsHelper->getWindowYPos() + mButtons[0]->getY(),
        mButtons[0]->getWidth(),
        mButtons[0]->getHeight());
    const bool IS_PIN_BOTTON_CLICKABLE =
        mXHelper->isWindowClickableInPinButton(
            getX11Window(), PIN_BUTTON_RECT, CURSOR_ROOT_POS);

    const bool PIN_VISIBILITY = mSettingsHelper->getBoolSetting(
        SettingsHelper::ENABLE_PIN_CONTROL) ||
        IS_PIN_BOTTON_CLICKABLE;
    setHoveredPinButtonVisibility(PIN_VISIBILITY);

    // Set visibility for all other controls.
    setHoveredControlButtonVisibility(CURSOR_ROOT_POS);
}

/**
 * Set visibility state of the four corner control buttons on
 * or off based on ConfigMode and update auto hide timer.
 */
void
StickyWindow::setControlButtonsVisibility() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);
    const bool CONFIG_MODE = mSettingsHelper->getConfigMode();

    // Pin button = 0, controls start @ 1;
    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setVisible(CONFIG_MODE);
    }
    draw();

    // If not Config mode, stop autohide timer & done.
    if (!CONFIG_MODE) {
        if (mAutoHideControlsTimer) {
            mAutoHideControlsTimer->stop();
        }
        return;
    }

    // If using autohide Config buttons, start timer.
    if (mSettingsHelper->getBoolSetting(SettingsHelper::
        AUTOHIDE_CONTROLS)) {
        mAutoHideControlsTimer->start(mSettingsHelper->getIntSetting(
            SettingsHelper::AUTOHIDE_DELAY) * 1000);
    }
}

/**
 * Setter for Hovered PinButton visibility state.
 */
void
StickyWindow::setHoveredPinButtonVisibility(
    const bool visibility) {

    if (mPinButton->isVisible() != visibility) {
        mPinButton->setVisible(visibility);
        draw();
    }
}

/**
 * Setter for all other Hovered ControlButton visibility state.
 */
void
StickyWindow::setHoveredControlButtonVisibility(
    const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    // Pin button = 0, controls start @ 1;
    const bool CONFIG_MODE = mSettingsHelper->getConfigMode();

    bool redrawRequired = false;

    // Set visible hovered, clear all others.
    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        const QRect BUTTON_RECT = QRect(
            mSettingsHelper->getWindowXPos() + mButtons[i]->getX(),
            mSettingsHelper->getWindowYPos() + mButtons[i]->getY(),
            mButtons[i]->getWidth(), mButtons[i]->getHeight());

        const bool PIN_VISIBILITY =
            mXHelper->isWindowClickableInControlButton(getX11Window(),
                BUTTON_RECT, position);

        if (PIN_VISIBILITY) {
            if (!mButtons[i]->isVisible()) {
                mButtons[i]->setVisible(true);
                redrawRequired = true;
            }
        } else {
            if (mButtons[i]->isVisible() != CONFIG_MODE) {
                mButtons[i]->setVisible(CONFIG_MODE);
                redrawRequired = true;
            }
        }
    }

    // One final draw for any affected.
    if (redrawRequired) {
        draw();
    }
}

/**
 * Press hovered button, & return it's position.
 */
QPoint
StickyWindow::pressHoveredButton(const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->getQRect().contains(position)) {
            mButtons[i]->setPressed(true);
            return QPoint(mButtons[i]->getX(),
                mButtons[i]->getY());
        }
    }
    return INVALID_POINT;
}

/**
 * Return draggable status of pressed button.
 */
bool
StickyWindow::isPressedButtonDraggable() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->isPressed()) {
            return mButtons[i]->isDraggable();
        }
    }

    return false;
}

/**
 * Return sizeable status of pressed button.
 */
bool
StickyWindow::isPressedButtonSizable() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->isPressed()) {
            return mButtons[i]->isSizeable();
        }
    }

    return false;
}

/**
 * Click previously pressed, hovered button.
 */
void
StickyWindow::clickPressedHoveredButton(const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->getQRect().contains(position)) {
            if (mButtons[i]->isPressed()) {
                if (i == 0) {
                    clickPressedPinButton();
                    return;
                }
                mButtons[i]->click(mX11Window);
                return;
            }
        }
    }
}

/**
 * If Pin button clicked, toggle config mode.
 */
void
StickyWindow::clickPressedPinButton() {
    // Toggle current to new mode.
    const bool CONFIG_MODE = mSettingsHelper->getConfigMode();
    const bool NEW_CONFIG_MODE = !CONFIG_MODE;
    mSettingsHelper->setConfigMode(NEW_CONFIG_MODE);

    setControlButtonsVisibility();
}

/**
 * Unpress all UI Buttons.
 */
void
StickyWindow::unPressAllWindowButtons() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setPressed(false);
    }
}

/**
 * Define Canvas position inside Window.
 */
void
StickyWindow::defineWindowCanvasPosition() {
    mSettingsHelper->setCanvasXPos(0);
    mSettingsHelper->setCanvasYPos(0);
}

/**
 * Define Canvas size inside Window.
 */
void
StickyWindow::defineWindowCanvasSize() {
    mSettingsHelper->setCanvasWidth(mSettingsHelper->getWindowWidth());
    mSettingsHelper->setCanvasHeight(mSettingsHelper->getWindowHeight());
}

/**
 * Main X11 Event handler.
 */
bool
StickyWindow::handleX11EventQueue() {
    // Event loop, until window close.
    XEvent event;
    while (XPending(mDisplay)) {
        XNextEvent(mDisplay, &event);
        switch (event.type) {

            // Detect root window or desktop property changes.
            case PropertyNotify:
                // For root window changes.
                if (event.xproperty.window == DefaultRootWindow(mDisplay)) {
                    // MAXIMUM DESKTOPS parm change.
                    if (event.xproperty.atom == XInternAtom(mDisplay,
                        "_NET_NUMBER_OF_DESKTOPS", False)) {
                        rangeCheckPreferredDesktopSetting();
                        updateActiveConfigDialog();
                        draw();
                        break;
                    }
                    // VISIBLE DESKTOP change.
                    if (event.xproperty.atom == XInternAtom(mDisplay,
                        "_NET_CURRENT_DESKTOP", False)) {
                        draw();
                        break;
                    }
                    break;
                }
                break;

            // ClientMsg says window close.
            case ClientMessage:
                // Our window needs updating with Config changes.
                if (event.xclient.message_type == mConfigUpdated) {
                    receiveConfigDialogUpdatedEvent(
                        event.xclient.data.l[0] ? true : false);
                    break;
                }
                // Our window wants close.
                if (static_cast<Atom>(event.xclient.data.l[0]) ==
                    mCloseAppMessage) {
                    return true;
                }
                break;

            // Type = Expose.
            case Expose:
                draw();
                break;

            // Type = UnmapNotify.
            case UnmapNotify:
                hide();
                break;

            // Type = MapNotify.
            case MapNotify:
                show();
                break;

            // Type = 22; ConfigureNotify.
            case ConfigureNotify:
                XTranslateCoordinates(mDisplay, event.xconfigure.window,
                    RootWindow(mDisplay, DefaultScreen(mDisplay)), 0, 0,
                    &mTranslatePosX, &mTranslatePosY, &mTranslateWindow);
                resize(mTranslatePosX, mTranslatePosY,
                    event.xconfigure.width, event.xconfigure.height);
                break;

            case MotionNotify:
                // Are we moving or sizing the window?
                if (mIsMouseClicked) {
                    if (isPressedButtonDraggable()) {
                        dragWindowToPoint(QPoint(event.xmotion.x_root,
                            event.xmotion.y_root));
                        break;
                    }
                    if (isPressedButtonSizable()) {
                        resizeWindowToPoint(QPoint(event.xmotion.x_root,
                            event.xmotion.y_root));
                        break;
                    }
                }
                break;

            case ButtonPress:
                // Grab the cursor on entry.
                mIsMouseClicked = true;

                XGrabPointer(mDisplay, mX11Window, False,
                    ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);

                // Save initial window size.
                mClickedWindowSize = QSize(
                    mSettingsHelper->getWindowWidth(),
                    mSettingsHelper->getWindowHeight());

                // Save click position, clicked button position,
                // and the offset of the two for the drag button
                // and for the move button.
                mClickedWindowPosition = QPoint(event.xbutton.x,
                    event.xbutton.y);
                mClickedButtonPosition = pressHoveredButton(
                    mClickedWindowPosition);
                if (mClickedButtonPosition == INVALID_POINT) {
                    break;
                }

                // Drag offset relative to top left button corner.
                mDragMoveButtonOffset = QPoint(
                    event.xbutton.x - mClickedButtonPosition.x(),
                    event.xbutton.y - mClickedButtonPosition.y());

                // Resize offset relative to bottom right button corner.
                mDragResizeButtonOffset = QPoint(
                    event.xbutton.x - mClickedButtonPosition.x() -
                        Button::BUTTON_WIDTH,
                    event.xbutton.y - mClickedButtonPosition.y() -
                        Button::BUTTON_HEIGHT
                );
                break;

            case ButtonRelease:
                // Release the cursor on exit.
                XUngrabPointer(mDisplay, CurrentTime);

                // Save final window size.
                mUnClickedWindowSize = QSize(
                    mSettingsHelper->getWindowWidth(),
                    mSettingsHelper->getWindowHeight());

                // If still hovering a control, release a click.
                mUnClickedWindowPosition = QPoint(event.xbutton.x,
                    event.xbutton.y);
                clickPressedHoveredButton(mUnClickedWindowPosition);

                if (mIsMovingWindow) {
                    mIsMovingWindow = false;
                    maybeAdjustWindowOverhang();
                }
                if (mIsSizingWindow) {
                    mIsSizingWindow = false;
                    if (mUnClickedWindowSize != mClickedWindowSize) {
                        mCanvas->uninitCanvas();
                        eraseWindow();
                        updateAllWindowButtons();
                        defineWindowCanvasPosition();
                        defineWindowCanvasSize();
                    }
                    draw();
                }

                unPressAllWindowButtons();
                mIsMouseClicked = false;
                break;
        }
    }
    return false;
}

/**
 * Ensure window opens on valid remembered desktop.
 */
void
StickyWindow::rangeCheckPreferredDesktopSetting() {
    // If we have no preferred desktop, no changes.
    const int PREFERRED_DESKTOP = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP);
    if (PREFERRED_DESKTOP == -1) {
        return;
    }

    // If preferred desktop no longer exists, set preferred to all.
    const int BOUNDED_PREFERRED_DESKTOP = PREFERRED_DESKTOP + 1;
    const int MAX_OS_DESKTOPS = mXHelper->getMaximumDesktops();
    if (BOUNDED_PREFERRED_DESKTOP > MAX_OS_DESKTOPS) {
        mSettingsHelper->setIntSetting(
            SettingsHelper::PREFERRED_DESKTOP, -1);
    }
}

/**
 * Update the Config Dialog if it's active &
 * the UI needs updating.
 */
void
StickyWindow::updateActiveConfigDialog() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->hasDialog()) {
            mButtons[i]->updateDialog();
            return;
        }
    }
}

/**
 * Receives an event from Qt ConfigDialog that
 * it needs to redraw canvas.
 */
void
StickyWindow::receiveConfigDialogUpdatedEvent(
    const bool canvasNeedsRedraw) {

    maybeAdjustWindowOverhang();
    setControlButtonsVisibility();
    setWindowStickPosition();
    rangeCheckPreferredDesktopSetting();

    if (canvasNeedsRedraw) {
        mCanvas->eraseCanvas();
        mCanvas->uninitCanvas();
        draw();
    }
}

/**
 * Perform window drag.
 */
void
StickyWindow::dragWindowToPoint(const QPoint position) {
    mIsMovingWindow = true;

    const bool ALLOW_DRAG_THRU_DESKTOPS = mSettingsHelper->
        getBoolSetting(SettingsHelper::ALLOW_DESKTOP_DRAG);
    const int VISIBLE_DESKTOP = mXHelper->getVisibleDesktop();
    const bool HAS_DESKTOP_PREFERENCE = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP) != -1;
    const int SCREEN_WIDTH = WidthOfScreen(
        DefaultScreenOfDisplay(mDisplay));
    const int KICK_DISTANCE = 2;

    // Create updatable position.
    QPoint dragPosition = position;

    // Check for drag left thru desktops, update position.
    if ((dragPosition.x() <= 0) && ALLOW_DRAG_THRU_DESKTOPS) {
        int desktop = VISIBLE_DESKTOP - 1;
        if (desktop < 0) {
            desktop = mXHelper->getMaximumDesktops() - 1;
        }

        // Drag from preferred desktop changes preferred desktop.
        if (HAS_DESKTOP_PREFERENCE) {
            mSettingsHelper->setIntSetting(SettingsHelper::
                PREFERRED_DESKTOP, desktop);
        }

        mXHelper->setVisibleDesktop(desktop);
        updateActiveConfigDialog();

        dragPosition.setX(SCREEN_WIDTH - KICK_DISTANCE);
        XWarpPointer(mDisplay, None, DefaultRootWindow(mDisplay),
            0, 0, 0, 0, dragPosition.x(), dragPosition.y());

    }

    // Check for drag right thru desktops, update position.
    if ((dragPosition.x() >= SCREEN_WIDTH - 1) &&
        ALLOW_DRAG_THRU_DESKTOPS) {
        int desktop = VISIBLE_DESKTOP + 1;
        if (desktop >= mXHelper->getMaximumDesktops()) {
            desktop = 0;
        }

        // Drag from preferred desktop changes preferred desktop.
        if (HAS_DESKTOP_PREFERENCE) {
            mSettingsHelper->setIntSetting(
                SettingsHelper::PREFERRED_DESKTOP, desktop);
        }

        mXHelper->setVisibleDesktop(desktop);
        updateActiveConfigDialog();

        dragPosition.setX(KICK_DISTANCE);
        XWarpPointer(mDisplay, None, DefaultRootWindow(mDisplay),
            0, 0, 0, 0, dragPosition.x(), dragPosition.y());
    }

    // Set final position & move there.
    mSettingsHelper->setWindowXPos(dragPosition.x() -
        mDragMoveButtonOffset.x());
    mSettingsHelper->setWindowYPos(dragPosition.y() -
        mDragMoveButtonOffset.y());

    XMoveWindow(mDisplay, mX11Window, mSettingsHelper->
        getWindowXPos(), mSettingsHelper->getWindowYPos());
}

/**
 * Perform window resizing.
 */
void
StickyWindow::resizeWindowToPoint(const QPoint position) {
    mIsSizingWindow = true;

    // Don't allow sizing below window minimum.
    const QRect ORIGINAL_WINDOW = QRect(QPoint(mSettingsHelper->
        getWindowXPos(), mSettingsHelper->getWindowYPos()),
        QSize(mSettingsHelper->getWindowWidth(), mSettingsHelper->
        getWindowHeight())
    );
    const QPoint ORIGINAL_POSITION = ORIGINAL_WINDOW.topLeft();
    const QPoint DIFF_POSITION = position - ORIGINAL_POSITION;

    // Get new window size, enforcing minimum and screen edge.
    QSize newWindowSize = QSize(DIFF_POSITION.x() -
        mDragResizeButtonOffset.x(), DIFF_POSITION.y() -
        mDragResizeButtonOffset.y());

    // Enforce window min & max width.
    const double SCREEN_WIDTH = WidthOfScreen(
        DefaultScreenOfDisplay(mDisplay));
    const double NEW_WINDOW_WIDTH = newWindowSize.width();
    newWindowSize.setWidth(min(max(mSettingsHelper->
        getWindowMinimumWidth(), NEW_WINDOW_WIDTH), SCREEN_WIDTH));

    // Enforce window min & max height.
    const double SCREEN_HEIGHT = HeightOfScreen(
        DefaultScreenOfDisplay(mDisplay));
    const double NEW_WINDOW_HEIGHT = newWindowSize.height();
    newWindowSize.setHeight(min(max(mSettingsHelper->
        getWindowMinimumHeight(), NEW_WINDOW_HEIGHT), SCREEN_HEIGHT));

    // Enforce window edges.
    const int NEW_WINDOW_RIGHT_POS = mSettingsHelper->getWindowXPos() +
        newWindowSize.width();
    if (NEW_WINDOW_RIGHT_POS > SCREEN_WIDTH) {
        newWindowSize.setWidth(SCREEN_WIDTH - mSettingsHelper->
            getWindowXPos());
    }
    const int NEW_WINDOW_BOTTOM_POS = mSettingsHelper->getWindowYPos() +
        newWindowSize.height();
    if (NEW_WINDOW_BOTTOM_POS > SCREEN_HEIGHT) {
        newWindowSize.setHeight(SCREEN_HEIGHT - mSettingsHelper->
            getWindowYPos());
    }

    // All is good, resize the window with new size.
    mSettingsHelper->setWindowWidth(newWindowSize.width());
    mSettingsHelper->setWindowHeight(newWindowSize.height());

    XResizeWindow(mDisplay, mX11Window, mSettingsHelper->
        getWindowWidth(), mSettingsHelper->getWindowHeight());

    // Resize canvas.
    updateAllWindowButtons();
    defineWindowCanvasPosition();
    defineWindowCanvasSize();

    // Re-draw all & done.
    draw();
}

/**
 * Determine if the current StickyWindow overhanges the screen
 * (current desktop) edges and moves it entirely into window
 * according to user pref.
 */
void
StickyWindow::maybeAdjustWindowOverhang() {
    // Determine screen size.
    const int SCREEN_WIDTH = WidthOfScreen(DefaultScreenOfDisplay(
        mDisplay));
    const int SCREEN_HEIGHT = HeightOfScreen(DefaultScreenOfDisplay(
        mDisplay));

    // Determine window top-left corner.
    QPoint* windowTopLeft = new QPoint(
        mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos());

    // Determine window bottom-right corner.
    QPoint* windowBottomRight = new QPoint(
        windowTopLeft->x() + mSettingsHelper->getWindowWidth() - 1,
        windowTopLeft->y() + mSettingsHelper->getWindowHeight() - 1);

    // Check for window overhang of desktop in left, top, &
    // right, bottom directions.
    bool doesWindowOverhang = false;

    if (windowTopLeft->x() < 0) {
        doesWindowOverhang = true;
        const double LEFT_OVERHANG = -windowTopLeft->x();
        windowTopLeft->setX(windowTopLeft->x() + LEFT_OVERHANG);
        windowBottomRight->setX(windowBottomRight->x() + LEFT_OVERHANG);
    }
    if (windowTopLeft->y() < 0) {
        doesWindowOverhang = true;
        const double TOP_OVERHANG = -windowTopLeft->y();
        windowTopLeft->setY(windowTopLeft->y() + TOP_OVERHANG);
        windowBottomRight->setY(windowBottomRight->y() + TOP_OVERHANG);
    }
    if (windowBottomRight->x() >= SCREEN_WIDTH) {
        doesWindowOverhang = true;
        const double RIGHT_OVERHANG = windowBottomRight->x() -
            SCREEN_WIDTH + 1;
        windowTopLeft->setX(windowTopLeft->x() - RIGHT_OVERHANG);
        windowBottomRight->setX(windowBottomRight->x() - RIGHT_OVERHANG);
    }
    if (windowBottomRight->y() >= SCREEN_HEIGHT) {
        doesWindowOverhang = true;
        const double BOTTOM_OVERHANG = windowBottomRight->y() -
            SCREEN_HEIGHT + 1;
        windowTopLeft->setY(windowTopLeft->y() - BOTTOM_OVERHANG);
        windowBottomRight->setY(windowBottomRight->y() - BOTTOM_OVERHANG);
    }

    // If the window overhangs the desktop & it's not allowed, move to
    // the safest position where all is on screen. resizeWindowToPoint()
    // ensures the window is always smaller than screen / desktop size.
    if (doesWindowOverhang) {
        const bool ALLOW_OVERHANG = mSettingsHelper->
            getBoolSetting(SettingsHelper::DESKTOP_OVERHANG);
        if (!ALLOW_OVERHANG) {
            mSettingsHelper->setWindowXPos(windowTopLeft->x());
            mSettingsHelper->setWindowYPos(windowTopLeft->y());
            XMoveWindow(mDisplay, mX11Window,
                mSettingsHelper->getWindowXPos(),
                mSettingsHelper->getWindowYPos());
        }
    }
}
