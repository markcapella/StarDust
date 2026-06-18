
#pragma once

/**
 * Display helper to check and/or return a valid x11
 * display & session type.
 *
 */
class DisplayHelper {

    public:
        DisplayHelper();

        /**
         * Return the Display*.
         */
        Display* getDisplay();

    private:
        // Members.
        Display* mDisplay = nullptr;
};
