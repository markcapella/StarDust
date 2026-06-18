
#pragma once

/**
 * Star(s) are the main objects in the view.
 */
class Star {

    public:
        // Defined in SettingsHelper.
        static inline const int AVAILABLE_STAR_COLORS = 4;

        // Constructor.
        Star(const Window window);

        /**
         * Getters & setters for class atrributes.
         */
        int getSize() const { return mSize; }
        void setSize(const int size) { mSize = size; }

        int getXPos() const { return mPosition.x(); }
        void setXPos(const int pos) { mPosition.setX(pos); }

        int getYPos() const { return mPosition.y(); }
        void setYPos(const int pos) { mPosition.setY(pos); }

        XRenderColor getColor() const { return mColor; }
        void setColor(const XRenderColor color) { mColor = color; }

        bool isVisible() const { return mIsVisible; }
        void setVisibility(const bool visibility) {
            mIsVisible = visibility;
        }

        /**
         * Set this star @ a random screen size.
         */
        void randomizeSize();

        /**
         * Set this star @ a random screen position.
         */
        void randomizePosition();

        /**
         * Set this star @ a random screen color.
         */
        void randomizeColor();

        /**
         * Check if this star has changed and needs to be redrawn.
         */
        bool needsDrawAfterChange() const {
            return mNeedsDrawAfterChange;
        }

        /**
         * Set this star has changed and needs to be redrawn.
         */
        void setNeedsDrawAfterChange(const bool value) {
            mNeedsDrawAfterChange = value;
        }

        /**
         * Draw this star.
         */
        void draw();

        /**
         * Erase this star.
         */
        void erase();

        /**
         * Create & start this Stars timer objects.
         */
        void createAndStartChangeTimers();

        /**
         * Restart this stars change timers after being stopped.
         */
        void startChangeTimers();

        /**
         * Stop this stars change timers after being started.
         */
        void stopChangeTimers();

        /**
         * Update this star @ a random screen size from
         * its current size.
         */
        void changeSize();

        /**
         * Update this star @ a random screen position from
         * its current position.
         */
        void changePosition();

        /**
         * Update this star @ a random screen color from
         * its current color.
         */
        void changeColor();

    private:
        // Members.
        Window mWindow = None;

        // Current star visibility.
        bool mIsVisible = false;
        bool mNeedsDrawAfterChange = false;

        // Color, size & position.
        XRenderColor mColor{};
        int mSize = -1;
        QPoint mPosition{};

        // Attribute change timers.
        unique_ptr<QTimer> mSizeChangeTimer{nullptr};
        unique_ptr<QTimer> mPositionChangeTimer{nullptr};
        unique_ptr<QTimer> mColorChangeTimer{nullptr};

};
