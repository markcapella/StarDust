
#include "Global.h"

/**
 * Canvas is the main widget & draw.
 */
Canvas::Canvas(const Window window) {
    mWindow = window;
}

Canvas::~Canvas() {
    uninitCanvas();
}

/**
 * Init/reinit() for the widget canvas.
 */
void
Canvas::initCanvas() {
    mStars.clear();

    const int NUMBER_OF_NEW_STARS = getSaturatedStarCount();
    for (int i = 0; i < NUMBER_OF_NEW_STARS; i++) {
        mStars.push_back(new Star(mWindow));
        mStars[i]->startChangeTimers();
    }

    mInitialized = true;
}

/**
 * Widget drawCanvas method.
 */
void
Canvas::drawCanvas() {
    if (!mInitialized) {
        initCanvas();
    }

    XRenderPictureAttributes polyEdgeSmooth{};
    polyEdgeSmooth.poly_edge = PolyEdgeSmooth;
    Picture canvasPic = XRenderCreatePicture(mDisplay,
        mWindow, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &polyEdgeSmooth);

    const XRenderColor BACKGROUND_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::BACKGROUND_COLOR);
    const int BACKGROUND_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::BACKGROUND_OPACITY);
    const XRenderColor BLENDED_BACKGROUND = newRenderColor(
        BACKGROUND_COLOR.red, BACKGROUND_COLOR.green,
        BACKGROUND_COLOR.blue, BACKGROUND_OPACITY);

    XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
        &BLENDED_BACKGROUND, mSettingsHelper->getCanvasXPos(),
        mSettingsHelper->getCanvasYPos(), mSettingsHelper->
        getCanvasWidth(), mSettingsHelper->getCanvasHeight());

    // Draw Widget canvas.
    const int NUMBER_OF_NEW_STARS = mStars.size();
    for (int i = 0; i < NUMBER_OF_NEW_STARS; i++) {
        //if (mStars[i]->needsDrawAfterChange()) {
            mStars[i]->draw();
       //}
    }
    setNeedsDrawAfterStarChange(false);

    // End transparent rendering.
    XRenderFreePicture(mDisplay, canvasPic);
    XFlush(mDisplay);
}

/**
 * Widget drawCanvas method.
 */
void
Canvas::eraseCanvas() {
    XRenderPictureAttributes polyEdgeSmooth{};
    polyEdgeSmooth.poly_edge = PolyEdgeSmooth;

    Picture canvasPic = XRenderCreatePicture(mDisplay,
        mWindow, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &polyEdgeSmooth);

    // Erase canvas stars.
    const int NUMBER_OF_NEW_STARS = mStars.size();
    for (int i = 0; i < NUMBER_OF_NEW_STARS; i++) {
        if (mStars[i]->isVisible()) {
            mStars[i]->erase();
        }
    }

    // Erase canvas.
    XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
        &TRANSPARENT_RCOLOR,
        mSettingsHelper->getCanvasXPos(),
        mSettingsHelper->getCanvasYPos(),
        mSettingsHelper->getCanvasWidth(),
        mSettingsHelper->getCanvasHeight()
    );

    // End transparent rendering.
    XRenderFreePicture(mDisplay, canvasPic);
    XFlush(mDisplay);
}

/**
 * Uninit for the widget canvas.
 */
void
Canvas::uninitCanvas() {
    if (!mInitialized) {
        return;
    }

    const int NUMBER_OF_NEW_STARS = mStars.size();
    for (int i = 0; i < NUMBER_OF_NEW_STARS; i++) {
        mStars[i]->stopChangeTimers();
        if (mStars[i]->isVisible()) {
            mStars[i]->erase();
        }
        delete mStars[i];
    }
    mStars.clear();

    mInitialized = false;
}

/**
 * Get number of stars maximum on field.
 */
int
Canvas::getSaturatedStarCount() {
    // Min & max of MAX_STAR_SIZE range.
    const float MIN_STAR_SIZE = mSettingsHelper->
        getSettingsIntRangeMinimum(SettingsHelper::MAX_STAR_SIZE);
    const float MAX_STAR_SIZE = mSettingsHelper->
        getSettingsIntRangeMaximum(SettingsHelper::MAX_STAR_SIZE);
    const float AVG_STAR_SIZE = (MAX_STAR_SIZE - MIN_STAR_SIZE) / 2;

    const float AVERAGE_STARS_ALONG_WIDTH = mSettingsHelper->
        getCanvasWidth() / AVG_STAR_SIZE;
    const float AVERAGE_STARS_ALONG_HEIGHT = mSettingsHelper->
        getCanvasHeight() / AVG_STAR_SIZE;
    const float SANE_DIVIDER = 0.25;

    const float SATURATION_PERCENT_DESIRED = (float) mSettingsHelper->
        getIntSetting(SettingsHelper::STAR_SATURATION) / 100;
    const float SATURATION_COUNT = AVERAGE_STARS_ALONG_WIDTH *
        AVERAGE_STARS_ALONG_HEIGHT * SANE_DIVIDER *
        SATURATION_PERCENT_DESIRED;

    /*
    cout << "MIN_STAR_SIZE" << MIN_STAR_SIZE << "." << endl;
    cout << "MAX_STAR_SIZE" << MAX_STAR_SIZE << "." << endl;
    cout << "AVG_STAR_SIZE" << AVG_STAR_SIZE << "." << endl;

    cout << "AVERAGE_STARS_ALONG_WIDTH" <<
        AVERAGE_STARS_ALONG_WIDTH << "." << endl;
    cout << "AVERAGE_STARS_ALONG_HEIGHT" <<
        AVERAGE_STARS_ALONG_HEIGHT << "." << endl;
    cout << "SANE_DIVIDER" << SANE_DIVIDER << "." << endl;

    cout << "SATURATION_PERCENT_DESIRED" <<
        SATURATION_PERCENT_DESIRED << "." << endl;
    cout << "SATURATION_COUNT" << SATURATION_COUNT << "." << endl;
    */

    return (int) SATURATION_COUNT;
}
