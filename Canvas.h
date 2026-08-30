
#pragma once

#include "Button.h"

/**
 * Canvas is the main widget & draw.
 */
class Canvas {

    public:
        Canvas(const Window window, const Picture picture,
            const std::vector<Button*>& buttons);

        ~Canvas();

        /**
         * Init for the widget canvas.
         */
        void initCanvas();

        /**
         * Invalidate current canvas, forcing a redraw.
         */
        bool isCanvasVisible() const { return mIsVisible; };

        /**
         * Invalidate current canvas, forcing a redraw.
         */
        void setCanvasHidden() { mIsVisible = false; };

        /**
         * Invalidate current canvas, forcing a redraw.
         */
        void setCanvasVisible() { mIsVisible = true; };

        /**
         * Draw the widget canvas, assumes cleared
         * transparent background.
         */
        void drawCanvas();

        /**
         * Erase the widget canvas.
         */
        void eraseCanvas();

        /**
         * Uninit for the widget canvas.
         */
        void uninitCanvas();

        /**
         * After a star erases itself, redraw all stars intersecting
         * it to correct visual artifacts.
         */
        void redrawIntersectingStars(Star* erasingStar);

    private:
        /**
         * Get number of stars maximum on field.
         */
        int getSaturatedStarCount();

        /**
         * Members.
         */
        Window mWindow = None;
        vector<Button*> mWindowButtons;
        Picture mRenderPicture{};

        bool mInitialized = false;
        bool mIsVisible = false;

        vector<Star*> mStars;
};
