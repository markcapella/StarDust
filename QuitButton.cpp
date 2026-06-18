
#include "Global.h"

/**
 * Simple class to represent a QuitButton.
 *
 */

/**
 * Draws the QuitButton.
 */
void
QuitButton::draw(const Window window) {
    XRenderPictureAttributes polyEdgeSmooth{};
    polyEdgeSmooth.poly_edge = PolyEdgeSmooth;
    Picture canvasPic = XRenderCreatePicture(mDisplay,
        window, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &polyEdgeSmooth);

    // Draw a white background, with a blue margin indented.
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getHeight());
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 1, getY() + 1,
        getWidth() - 2, getHeight() - 2);

    // White or grayed button center as pressed.
    const XRenderColor BUTTON_COLOR = isPressed() ?
        GRAY_RCOLOR : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BUTTON_COLOR, getX() + 3 , getY() + 3,
        getWidth() - 6, getHeight() - 6);

    // Draw 'X' for Close glyph over center.
    const int PIXEL_SIZE = 2;
    for (int i = 6; i <= getWidth() - 6 - PIXEL_SIZE; i++) {
        XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
            &BLUE_RCOLOR, getX() + i, getY() + i,
                PIXEL_SIZE, PIXEL_SIZE);
        XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
            &BLUE_RCOLOR, getX() + i, getY() + getHeight() - i - PIXEL_SIZE,
                PIXEL_SIZE, PIXEL_SIZE);
    }

    XRenderFreePicture(mDisplay, canvasPic);
}

/**
 * Erase the Button.
 */
void
QuitButton::erase(const Window window) {
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
 * Clicks the QuitButton.
 */
void
QuitButton::click(const Window window) {
    const Atom WM_PROTOCOLS = XInternAtom(mDisplay,
        "WM_PROTOCOLS", False);
    const Atom WM_DELETE_WINDOW = XInternAtom(mDisplay,
        "WM_DELETE_WINDOW", False);

    XEvent event{}; 
    event.xclient.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = WM_PROTOCOLS;

    event.xclient.format = 32;
    event.xclient.data.l[0] = static_cast<long>(WM_DELETE_WINDOW);
    event.xclient.data.l[1] = CurrentTime;

    XSendEvent(mDisplay, window, False, NoEventMask, &event);
    XFlush(mDisplay);
}

/**
 * Updates any Button Dialog.
 */
void
QuitButton::updateDialog() {
    // Nothing.
}
