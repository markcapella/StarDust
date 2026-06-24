
#pragma once

/**
 * Canvas is the main widget & draw.
 */
class Canvas {

    public:
        Canvas(const Window window);
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
         * Check if stars have changed and need to be redrawn.
         */
        bool needsDrawAfterStarChange() const {
            return mNeedsDrawAfterStarChange;
        }

        /**
         * Set that stars have been changed and need to be redrawn.
         */
        void setNeedsDrawAfterStarChange(const bool value) {
            mNeedsDrawAfterStarChange = value;
        }

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

        bool mInitialized = false;
        bool mIsVisible = false;
        bool mNeedsDrawAfterStarChange = false;

        vector<Star*> mStars;

        /**
         * Get number of stars maximum on field.
         */
        int getSaturatedStarCount();
};
