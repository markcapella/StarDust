
#pragma once

#include "Button.h"

/**
 * Canvas is the main widget & draw.
 */
class Canvas {

    public:
        Canvas(const Window window, const std::vector<Button*>& buttons);

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
        void setCanvasVisibile() { mIsVisible = true; };

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

    private:
        // Members.
        Window mWindow = None;
        vector<Button*> mWindowButtons;

        bool mInitialized = false;
        bool mIsVisible = false;

        vector<Star*> mStars;

        /**
         * Get number of stars maximum on field.
         */
        int getSaturatedStarCount();
};
