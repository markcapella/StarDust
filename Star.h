
#pragma once

#include <X11/extensions/Xrender.h>

#include "Button.h"

#include "StarImage.h"
#include "StarImageAmerican.h"
#include "StarImageCapella.h"
#include "StarImagePatrick.h"
#include "StarImageThrowing.h"
#include "StarImageTrek.h"
#include "AllStarImageCollection.h"

/**
 * Star(s) are the main objects in the view.
 */
class Star : public QObject {
    Q_OBJECT

    public:
        static inline constexpr QPoint INVALID_POSITION { -1, -1 };

        // Defined in SettingsHelper.
        static inline const int AVAILABLE_STAR_COLORS = 4;

        static inline const XRenderColor STAR_COLOR = {
            .red = 0x0000, .green = 0xffff, .blue = 0xffff,
            .alpha = 0xffff };

        const int STAR_SIZE = 26;

        // Constructor.
        explicit Star(const Window window, const Picture picture,
            const std::vector<Button*> buttons, QObject* parent = nullptr);

        ~Star();

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
         * Draw this star.
         */
        void draw();

        /**
         * Erase this star.
         */
        void erase();

        /**
         * Restart this stars change timers after being stopped.
         */
        void startChangeTimers();

        /**
         * Stop this stars change timers after being started.
         */
        void stopChangeTimers();

    private:
        /**
         * Create & start this Stars timer objects.
         */
        void createAndStartChangeTimers();

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

        /**
         * Set StarImage mono Picture from mono color images array.
         */
        void setStarImageMonoPicture(const StarImage& image);

        /**
         * Set StarImage color Picture, by applying it's color
         * to its mono Picture.
         */
        void setStarImageColorPicture();

        /**
         * Debug a StarImage's pixel data.
         */
        void debugStarImage(const StarImage& image);

        /**
         * Members.
         */
        Window mWindow = None;
        Picture mRenderPicture{};
        std::vector<Button*> mWindowButtons;

        Picture mStarImageMonoPicture{};
        Picture mStarImageColorPicture{};

        // Current star visibility.
        bool mIsVisible = false;

        // Color, size & position.
        XRenderColor mColor{};
        int mSize = -1;
        QPoint mPosition{ -1, -1 };

        // Attribute change timers.
        QTimer* mSizeChangeTimer = nullptr;
        QTimer* mPositionChangeTimer = nullptr;
        QTimer* mColorChangeTimer = nullptr;

        int mDebugCount = 0;
};
