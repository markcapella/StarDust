
#include "Global.h"

/**
 * Star(s) are the main objects in the view.
 */
Star::Star(const Window window,  const Picture picture,
    const std::vector<Button*> buttons, QObject* parent) :
    QObject(parent) {

    mWindow = window;
    mRenderPicture = picture;
    mWindowButtons = buttons;

    randomizeSize();
    randomizePosition();
    randomizeColor();

    // Start change timers and wait for twinkle.
    createAndStartChangeTimers();
}

/**
 * Destructor.
 */
Star::~Star() {
    if (mStarImageMonoPicture) {
        XRenderFreePicture(mDisplay, mStarImageMonoPicture);
        mStarImageMonoPicture = {};
    }

    if (mStarImageColorPicture) {
        XRenderFreePicture(mDisplay, mStarImageColorPicture);
        mStarImageColorPicture = {};
    }
}

/**
 * Draw this star.
 */
void
Star::draw() {
    lock_guard<recursive_mutex> lock(gX11Mutex);

    // Avoid drawing in visible corners.
    const int LEFT = getXPos();
    const int TOP = getYPos();

    const QRect STAR_RECT(LEFT, TOP, mSize, mSize);
    const int BUTTONS_COUNT = mWindowButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        const QRect BUTTON_RECT =
            mWindowButtons[i]->getQRect();
        if (BUTTON_RECT.intersects(STAR_RECT)) {
            mIsVisible = false;
            return;
        }
    }

    XRenderComposite(mDisplay, PictOpOver, mStarImageColorPicture,
        mStarImageMonoPicture, mRenderPicture, 0, 0, 0, 0,
        LEFT, TOP, Star::mSize, Star::mSize);
    XSync(mDisplay, False);

    mIsVisible = true;
}

/**
 * Erase this star.
 */
void
Star::erase() {
    if (!mIsVisible) {
        return;
    }

    lock_guard<recursive_mutex> lock(gX11Mutex);

    const XRenderColor BACKGROUND_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::BACKGROUND_COLOR);

    const int BACKGROUND_OPACITY = mSettingsHelper->getIntSetting(
        SettingsHelper::BACKGROUND_OPACITY);

    const XRenderColor STAR_RCOLOR = newRenderColor(
        BACKGROUND_COLOR.red, BACKGROUND_COLOR.green,
        BACKGROUND_COLOR.blue, BACKGROUND_OPACITY);

    XRenderFillRectangle(mDisplay, PictOpSrc, mRenderPicture,
        &STAR_RCOLOR, getXPos(), getYPos(), mSize, mSize);

    // Safely trigger neighbor redraws while locked
    if (mCanvas) {
        mCanvas->redrawIntersectingStars(this);
    }

    mIsVisible = false;
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

    delete mSizeChangeTimer;
    delete mPositionChangeTimer;
    delete mColorChangeTimer;
}

/**
 * Create & start this Stars timer objects.
 */
void
Star::createAndStartChangeTimers() {
    // Create & start Size Change timer.
    mSizeChangeTimer = new QTimer(this);
    mSizeChangeTimer->setInterval(1);
    connect(mSizeChangeTimer, &QTimer::timeout, this, [this]() {
        changeSize();
    });
    QTimer::singleShot(randomIntegerUpTo(500), this, [this]() {
        mSizeChangeTimer->start(); });

    // Create & start Position Change timer.
    mPositionChangeTimer = new QTimer(this);
    mPositionChangeTimer->setInterval(100);
    connect(mPositionChangeTimer, &QTimer::timeout, this, [this]() {
        changePosition();
    });
    QTimer::singleShot(randomIntegerUpTo(500), this, [this]() {
        mPositionChangeTimer->start(); });

    // Create & start Color Change timer.
    mColorChangeTimer = new QTimer(this);
    mColorChangeTimer->setInterval(100);
    connect(mColorChangeTimer, &QTimer::timeout, this, [this]() {
        changeColor();
    });
    QTimer::singleShot(randomIntegerUpTo(500), this, [this]() {
        mColorChangeTimer->start(); });
}

/**
 * Set this star @ a random screen size.
 */
void
Star::randomizeSize() {
    const int MINIMUM = mSettingsHelper->getSettingsIntRangeMinimum(
        SettingsHelper::MAX_STAR_SIZE);
    const int MAXIMUM = mSettingsHelper->getIntSetting(
        SettingsHelper::MAX_STAR_SIZE);

    // First time, uninit-ed.
    if (mSize == -1) {
        mSize = randomIntegerUpTo(MAXIMUM - MINIMUM + 1) + MINIMUM;
        randomizePosition();

    // Else, random chance to decrease visual size.
    } else if (randomIntegerUpTo(2) == 0) {
        const int NEW_SIZE = mSize - 4;
        if (NEW_SIZE < MINIMUM) { return; } else {
            mSize = NEW_SIZE;
            if (mPosition != INVALID_POSITION) {
                setXPos(getXPos() + 2);
                setYPos(getYPos() + 2);
            }
        }

    // Else, increase visual size.
    } else {
        const int NEW_SIZE = mSize + 4;
        if (NEW_SIZE > MAXIMUM) { return; } else {
            mSize = NEW_SIZE;
            if (mPosition != INVALID_POSITION) {
                setXPos(getXPos() - 2);
                setYPos(getYPos() - 2);
            }
        }
    }

    // Set new mono star image.
    const QString STAR_IMAGE = mSettingsHelper->
        getStringSetting(SettingsHelper::STAR_IMAGE);
    const int STAR_IMAGE_INDEX = SettingsHelper::
        SettingsHelper::ALL_STAR_IMAGES.indexOf(STAR_IMAGE);
    setStarImageMonoPicture(AllStarImageCollection
        [STAR_IMAGE_INDEX][mSize - MINIMUM]
    );

    // Update it's color counterpart.
    setStarImageColorPicture();
}

/**
 * Set this star @ a random screen position.
 */
void
Star::randomizePosition() {
    // First time, set size, and guard position.
    if (mSize == -1) {
        const int MINIMUM = mSettingsHelper->
            getSettingsIntRangeMinimum(SettingsHelper::MAX_STAR_SIZE);
        const int MAXIMUM = mSettingsHelper->
            getIntSetting(SettingsHelper::MAX_STAR_SIZE);
        mSize = randomIntegerUpTo(MAXIMUM - MINIMUM) + MINIMUM;
    }

    // Then, pick a position for the size.
    const int CANVAS_X_POS = mSettingsHelper->getCanvasXPos();
    const int CANVAS_Y_POS = mSettingsHelper->getCanvasYPos();

    const int CANVAS_WIDTH = mSettingsHelper->getCanvasWidth();
    const int CANVAS_HEIGHT = mSettingsHelper->getCanvasHeight();

    setXPos(CANVAS_X_POS + randomIntegerUpTo(CANVAS_WIDTH - mSize));
    setYPos(CANVAS_Y_POS + randomIntegerUpTo(CANVAS_HEIGHT - mSize));
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

    setStarImageColorPicture();
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

    if (mCanvas->isCanvasVisible()) {
        if (mDebugCount < 99999) {
            mDebugCount++;
            erase();
            randomizeSize();
            draw();
        }
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

    if (mCanvas->isCanvasVisible()) {
        erase();
        randomizePosition();
        draw();
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

    if (mCanvas->isCanvasVisible()) {
        erase();
        randomizeColor();
        draw();
    }
}

/**
 * Set StarImage mono Picture from mono color images array.
 */
void
Star::setStarImageMonoPicture(const StarImage& starImage) {
    // Create XImage of StarImage size.
    XImage* starXImage = XCreateImage(mDisplay, DefaultVisual(mDisplay,
        DefaultScreen(mDisplay)), 8, ZPixmap, 0, nullptr,
        starImage.size, starImage.size, 8, starImage.size);
    if (!starXImage) {
        return;
    }

    // Add copy of starImage to XImage.
    const int STAR_DATA_SIZE = starImage.size * starImage.size;
    starXImage->data = new char[STAR_DATA_SIZE];
    memcpy(starXImage->data, starImage.pixels, STAR_DATA_SIZE);

    // Create Pixmap of StarImage size.
    Pixmap starPixmap = XCreatePixmap(mDisplay, DefaultRootWindow(
        mDisplay), starImage.size, starImage.size, 8);
    if (!starPixmap) {
        XDestroyImage(starXImage);
        return;
    }

    // Copy XImage into Pixmap.
    GC gc = XCreateGC(mDisplay, starPixmap, 0, nullptr);
    XPutImage(mDisplay, starPixmap, gc, starXImage, 0, 0, 0, 0,
        starImage.size, starImage.size);
    XFreeGC(mDisplay, gc);
    XDestroyImage(starXImage);

    // Create mono picture from Pixmap.
    const XRenderPictFormat* FORMAT = XRenderFindStandardFormat(
        mDisplay, PictStandardA8);
    const Picture STAR_MONO_PICTURE = XRenderCreatePicture(mDisplay,
        starPixmap, FORMAT, 0, nullptr);
    XFreePixmap(mDisplay, starPixmap);

    // Set result.
    if (mStarImageMonoPicture) {
        XRenderFreePicture(mDisplay, mStarImageMonoPicture);
    }
    mStarImageMonoPicture = STAR_MONO_PICTURE;
}

/**
 * Set StarImage color Picture, by applying it's color
 * to its mono Picture.
 */
void
Star::setStarImageColorPicture() {
    // Create this color picture.
    Pixmap starPixmap = XCreatePixmap(mDisplay, DefaultRootWindow(
        mDisplay), Star::mSize, Star::mSize, 32);
    const XRenderPictFormat* FORMAT = XRenderFindStandardFormat(
        mDisplay, PictStandardARGB32);
    const Picture STAR_COLOR_PICTURE = XRenderCreatePicture(
        mDisplay, starPixmap, FORMAT, 0, nullptr);
    XRenderFillRectangle(mDisplay, PictOpSrc, STAR_COLOR_PICTURE,
        &mColor, 0, 0, Star::mSize, Star::mSize);
    XFreePixmap(mDisplay, starPixmap);

    // Set result.
    if (mStarImageColorPicture) {
        XRenderFreePicture(mDisplay, mStarImageColorPicture);
    }
    mStarImageColorPicture = STAR_COLOR_PICTURE;
}

/**
 * Debug a StarImage's pixel data.
 */
void
Star::debugStarImage(const StarImage& starImage) {
    // Start a long output message string.
    QString outputMessage = QString("StarImage %1x%1:\n").
        arg(starImage.size);

    // Add all debug info to output message string.
    for (int y = 0; y < starImage.size; y++) {
        for (int x = 0; x < starImage.size; x++) {
            const unsigned V = starImage.pixels[y * starImage.size + x];
            outputMessage += QString("%1 ").arg(V, 2, 16, QChar('0'));
        }
        outputMessage += '\n';
    }

    // Log output message.
    cout << outputMessage.toStdString();
}
