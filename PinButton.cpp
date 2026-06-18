
#include "Global.h"

/**
 * Simple class to represent a PinButton.
 *
 */
PinButton::PinButton(double mXPos, double mYPos) :
    Button(mXPos, mYPos){}

// Provides dynamic_cast<PinButton*> type cast ability.
PinButton::~PinButton(){}

/**
 * Draws the PinButton.
 */
void
PinButton::draw(const Window window) {
    XRenderPictureAttributes polyEdgeSmooth{};
    polyEdgeSmooth.poly_edge = PolyEdgeSmooth;
    Picture canvasPic = XRenderCreatePicture(mDisplay, window,
        XRenderFindStandardFormat(mDisplay, PictStandardARGB32),
        CPPolyEdge, &polyEdgeSmooth);

    // Draw a white background.
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getHeight());

    // Display pinIn or pinOut png.
    GC gc = XCreateGC(mDisplay, window, 0, nullptr);
    XPutImage(mDisplay, window, gc, (mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE) ? mPinOutXImage : mPinInXImage),
            0, 0, getX() + 2, getY() + 2,
                getWidth() - 4, getHeight() - 4);
    XFreeGC(mDisplay, gc);

    XRenderFreePicture(mDisplay, canvasPic);
}

/**
 * Erase the Button.
 */
void
PinButton::erase(const Window window) {
    XRenderPictureAttributes polyEdgeSmooth{};
    polyEdgeSmooth.poly_edge = PolyEdgeSmooth;
    Picture canvasPic = XRenderCreatePicture(mDisplay, window,
        XRenderFindStandardFormat(mDisplay, PictStandardARGB32),
        CPPolyEdge, &polyEdgeSmooth);

    XRenderFillRectangle(mDisplay, PictOpSrc, canvasPic,
        &TRANSPARENT_RCOLOR, getX(), getY(), getWidth(), getHeight());

    XRenderFreePicture(mDisplay, canvasPic);
}

/**
 * Clicks the PinButton.
 */
// Suppress warnings based on Clang or GCC.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
PinButton::click(const Window window) {
    // Nothing.
}

#pragma GCC diagnostic pop

/**
 * Updates any Button Dialog.
 */
void
PinButton::updateDialog() {
    // Nothing.
}
