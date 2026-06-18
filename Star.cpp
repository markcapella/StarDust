
#include "Global.h"

/**
 * Star(s) are the main objects in the view.
 */
Star::Star(const Window window) {
    mWindow = window;

    // After setting initial size, we can randomize
    // position, then randomize color.
    randomizeSize();
    randomizePosition();
    randomizeColor();

    // Start change timers and wait for twinkle.
    createAndStartChangeTimers();
    setNeedsDrawAfterChange(true);
}

/**
 * Set this star @ a random screen size.
 */
void
Star::randomizeSize() {
    const int MAX_SIZE_DESIRED = mSettingsHelper->
        getIntSetting(SettingsHelper::MAX_STAR_SIZE);

    const int MAX_SIZE_MINIMUM = mSettingsHelper->
        getSettingsIntRangeMinimum(SettingsHelper::MAX_STAR_SIZE);
    const int MAX_RANGE = MAX_SIZE_DESIRED - MAX_SIZE_MINIMUM;

    mSize = MAX_SIZE_MINIMUM + randomIntegerUpTo(MAX_RANGE);
}

/**
 * Set this star @ a random screen position.
 */
void
Star::randomizePosition() {
    const int CANVAS_WIDTH_POS = mSettingsHelper->getCanvasXPos();
    const int CANVAS_WIDTH = mSettingsHelper->getCanvasWidth();

    const int CANVAS_HEIGHT_POS = mSettingsHelper->getCanvasYPos();
    const int CANVAS_HEIGHT = mSettingsHelper->getCanvasHeight();

    const int MAX_SIZE = mSettingsHelper->
        getIntSetting(SettingsHelper::MAX_STAR_SIZE);

    setXPos(CANVAS_WIDTH_POS + randomIntegerUpTo(
        CANVAS_WIDTH - MAX_SIZE));
    setYPos(CANVAS_HEIGHT_POS + randomIntegerUpTo(
        CANVAS_HEIGHT - MAX_SIZE));
}

/**
 * Set this star @ a random screen color.
 */
void
Star::randomizeColor() {
    switch (randomIntegerUpTo(AVAILABLE_STAR_COLORS)) {
        case 0: mColor = mSettingsHelper->getColorSetting(
            SettingsHelper::STAR_COLOR_COOL);
            break;
        case 1: mColor = mSettingsHelper->getColorSetting(
            SettingsHelper::STAR_COLOR_WARM);
            break;
        case 2: mColor = mSettingsHelper->getColorSetting(
            SettingsHelper::STAR_COLOR_MEDIUM);
            break;
        case 3: mColor = mSettingsHelper->getColorSetting(
            SettingsHelper::STAR_COLOR_HOT);
    }
}

/**
 * Draw this star.
 */
void
Star::draw() {
    Picture canvasPic = XRenderCreatePicture(mDisplay,
        mWindow, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), 0, nullptr);

    const XRenderColor STAR_RCOLOR = getColor();

    const int POS_OFFSET = (mSettingsHelper->getIntSetting(
        SettingsHelper::MAX_STAR_SIZE) - getSize()) / 2;

    for (int i = 0; i < getSize(); i++) {
        XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
            &STAR_RCOLOR, getXPos() + POS_OFFSET + i,
            getYPos() + POS_OFFSET + i, 1, 1);
        XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
            &STAR_RCOLOR, getXPos() + POS_OFFSET + i,
            getYPos() + POS_OFFSET + getSize() - i - 1, 1, 1);
    }

    const int MID_POINT = (getSize() - 1) / 2;
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &STAR_RCOLOR, getXPos() + POS_OFFSET + 1,
        getYPos() + POS_OFFSET + MID_POINT, getSize() - 2, 1);
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &STAR_RCOLOR, getXPos() + POS_OFFSET + MID_POINT,
        getYPos() + POS_OFFSET + 1, 1, getSize() - 2);

    // End transparent rendering.
    XRenderFreePicture(mDisplay, canvasPic);
    mIsVisible = true;
    setNeedsDrawAfterChange(false);
}

/**
 * Erase this star.
 */
void
Star::erase() {
    Picture canvasPic = XRenderCreatePicture(mDisplay,
        mWindow, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), 0, nullptr);

    const XRenderColor BACKGROUND_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::BACKGROUND_COLOR);
    const int BACKGROUND_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::BACKGROUND_OPACITY);
    const XRenderColor STAR_RCOLOR = newRenderColor(
        BACKGROUND_COLOR.red, BACKGROUND_COLOR.green,
        BACKGROUND_COLOR.blue, BACKGROUND_OPACITY);

    const int POS_OFFSET = (mSettingsHelper->getIntSetting(
        SettingsHelper::MAX_STAR_SIZE) - getSize()) / 2;

    for (int i = 0; i < getSize(); i++) {
        XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
            &STAR_RCOLOR, getXPos() + POS_OFFSET + i,
            getYPos() + POS_OFFSET + i, 1, 1);
        XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
            &STAR_RCOLOR, getXPos() + POS_OFFSET + i,
            getYPos() + POS_OFFSET + getSize() - i - 1, 1, 1);
    }

    const int MID_POINT = (getSize() - 1) / 2;
    XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
        &STAR_RCOLOR, getXPos() + POS_OFFSET + 1,
        getYPos() + POS_OFFSET + MID_POINT, getSize() - 2, 1);
    XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
        &STAR_RCOLOR, getXPos() + POS_OFFSET + MID_POINT,
        getYPos() + POS_OFFSET + 1, 1, getSize() - 2);

    // End transparent rendering.
    XRenderFreePicture(mDisplay, canvasPic);
    mIsVisible = false;
    setNeedsDrawAfterChange(false);
}

/**
 * Create & start this Stars timer objects.
 */
void
Star::createAndStartChangeTimers() {
    // Create change timers for size.
    mSizeChangeTimer = make_unique<QTimer>();
    mSizeChangeTimer->setInterval(100);
    QObject::connect(mSizeChangeTimer.get(), &QTimer::timeout,
        [this] () { changeSize(); });
    QTimer::singleShot(randomIntegerUpTo(500), [this]() {
        mSizeChangeTimer->start();
    });

    // Create change timer for position.
    mPositionChangeTimer = make_unique<QTimer>();
    mPositionChangeTimer->setInterval(100);
    QObject::connect(mPositionChangeTimer.get(), &QTimer::timeout,
        [this] () { changePosition(); });
    QTimer::singleShot(randomIntegerUpTo(500), [this]() {
        mPositionChangeTimer->start();
    });

    // Create change timer for color.
    mColorChangeTimer = make_unique<QTimer>();
    mColorChangeTimer->setInterval(100);
    QObject::connect(mColorChangeTimer.get(), &QTimer::timeout,
        [this] () { changeColor(); });
    QTimer::singleShot(randomIntegerUpTo(500), [this]() {
        mColorChangeTimer->start();
    });
}

/**
 * Restart this stars change timers after being stopped.
 */
void
Star::startChangeTimers() {
    mSizeChangeTimer->start();
    mPositionChangeTimer->start();
    mColorChangeTimer->start();
}

/**
 * Stop this stars change timers after being started.
 */
void
Star::stopChangeTimers() {
    mSizeChangeTimer->stop();
    mPositionChangeTimer->stop();
    mColorChangeTimer->stop();
}

/**
 * Update this star @ a random screen size from
 * its current size.
 */
void
Star::changeSize() {
    if (randomIntegerUpTo(mSettingsHelper->getIntSetting(
        SettingsHelper::SIZE_CHANGE_DELAY) + 1) != 0) {
        return;
    }

    if (mIsVisible) {
        erase();
    }
    if (mCanvas->isCanvasVisible()) {
        randomizeSize();
        draw();
        //setNeedsDrawAfterChange(true);
        //mCanvas->setNeedsDrawAfterStarChange(true);
    }
}

/**
 * Update this star @ a random screen position from
 * its current position.
 */
void
Star::changePosition() {
    if (randomIntegerUpTo(mSettingsHelper->getIntSetting(
        SettingsHelper::POSITION_CHANGE_DELAY) + 1) != 0) {
        return;
    }

    if (mIsVisible) {
        erase();
    }
    if (mCanvas->isCanvasVisible()) {
        randomizePosition();
        draw();
        //setNeedsDrawAfterChange(true);
        //mCanvas->setNeedsDrawAfterStarChange(true);
    }
}

/**
 * Update this star @ a random screen color from
 * its current color.
 */
void
Star::changeColor() {
    if (randomIntegerUpTo(mSettingsHelper->getIntSetting(
        SettingsHelper::COLOR_CHANGE_DELAY) + 1) != 0) {
        return;
    }

    if (mIsVisible) {
        erase();
    }
    if (mCanvas->isCanvasVisible()) {
        randomizeColor();
        draw();
        //setNeedsDrawAfterChange(true);
        //mCanvas->setNeedsDrawAfterStarChange(true);
    }
}
