
#pragma once

/**
 * Simple class to represent a PinButton.
 */
 class PinButton : public Button {
    public:
        PinButton(const double mXPos, const double mYPos);
        ~PinButton() override;

        void draw(const Window window) override;

        void erase(const Window window) override;

        void click(const Window window) override;

        void updateDialog() override;
};
