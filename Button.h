
#pragma once


/**
 * Simple class to represent an Button.
 *
 */
class Button {
    public:
        // Define Button.
        static inline const int BUTTON_WIDTH = 24;
        static inline const int BUTTON_HEIGHT = 24;

        Button(const double mXPos, const double mYPos) :
            mX(mXPos), mY(mYPos), mWidth(BUTTON_WIDTH),
            mHeight(BUTTON_HEIGHT){}

        virtual ~Button() = default;

        double getX() const { return mX; }
        void setX(const double xPos) { mX = xPos; }

        double getY() const { return mY; }
        void setY(const double yPos) { mY = yPos; }

        double getWidth() const { return mWidth; }
        void setWidth(const double width) { mWidth = width; }

        double getHeight() const { return mHeight; }
        void setHeight(const double height) { mHeight = height; }

        QRect getRect() const { return QRect(
            mX, mY, mWidth, mHeight); }

        bool isVisible() const { return mButtonVisible; }
        void setVisible(const bool visible) {
            mButtonVisible = visible; }

        bool isPressed() const { return mButtonPressed; }
        void setPressed(const bool pressed) {
            mButtonPressed = pressed; }

        bool isDraggable() const { return mButtonDraggable; }
        void setDraggable(const bool draggable) {
            mButtonDraggable = draggable; }

        bool isSizeable() const { return mButtonSizeable; }
        void setSizeable(const bool sizeable) {
            mButtonSizeable = sizeable; }

        bool hasDialog() const { return mButtonHasDialog; }
        void setHasDialog(const bool hasDialog) {
            mButtonHasDialog = hasDialog; }

        string toString() const {
            ostringstream outString;
            outString << "mX , mY : [" <<
                mX << " , " << mY << "], w , h : [" <<
                mWidth << " , " << mHeight << "].";
            return outString.str();
        }

        virtual void draw(const Window window) = 0;

        virtual void erase(const Window window) = 0;

        virtual void click(const Window window) = 0;

        virtual void updateDialog() = 0;

    private:
        double mX;
        double mY;
        double mWidth;
        double mHeight;

        bool mButtonVisible = false;
        bool mButtonPressed = false;
        bool mButtonDraggable = false;
        bool mButtonSizeable = false;
        bool mButtonHasDialog = false;
};
