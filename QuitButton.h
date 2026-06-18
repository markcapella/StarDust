
#pragma once


/**
 * Simple class to represent a QuitButton.
 *
 */
class QuitButton : public Button {
    public:
        QuitButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos){}

        void draw(const Window window) override;

        void erase(const Window window) override;

        void click(const Window window) override;

        void updateDialog() override;
};
