
#pragma once


/**
 * Simple class to represent a Button.
 *
 */
class ConfigButton : public Button {
    public:
        ConfigButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) {
            setHasDialog(true);
        }

        void draw(const Window window) override;

        void erase(const Window window) override;

        void click(const Window window) override;

        void updateDialog() override;

    private:
        bool mConfigShown = false;

        ConfigDialog* mConfigDialog = nullptr;
};
