
#pragma once


/**
 * Simple class to represent a SizeButton.
 *
 */
class SizeButton : public Button {
    public:
        SizeButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) {
            setSizeable(true);
        }

        void draw(const Window window) override;

        void erase(const Window window) override;

        void click(const Window window) override;

        void updateDialog() override;
};
