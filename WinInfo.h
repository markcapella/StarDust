
#pragma once

/**
 * Simple class to represent a Windows Info object.
 *
 */
class WinInfo {

    public:
        WinInfo(){}

        // Members.
        Window window = None;

        QRect windowRect{};

        int onWorkspace = -1; // All.
        int mapState = -1; // Undef.

        bool isSticky = false;
        bool isDock = false;
        bool isHidden = false;
};
