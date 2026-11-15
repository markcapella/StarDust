
#pragma once

/**
 * OpenboxHelper addresses differences in xfwm4 / kwin / mutter
 * Window Manager behaviors. Openbox has quirks for windows that
 * are DOCKS (undecorated windows such as StickWidgets) ...
 * ... in its "Stay on Top" or "Stay on Bottom" logic.
 *
 */
class OpenboxHelper {

    public:
        enum DockStacking {
            INDETERMINATE,
            KEEP_DOCK_ABOVE_OTHER_WINDOWS,
            ALLOW_DOCK_ABOVE_AND_BELOW_WINDOWS,
            KEEP_DOCK_BELOW_OTHER_WINDOWS
        };

        /**
         * Constructor.
         */
        OpenboxHelper();

        /**
         * Openbox won't move an undecorated DOCK WINDOW type to
         * Top or Bottom, so we do it ourselves.
         */
        void setWindowOnTopOrOnBottomForOpenbox(const Window window);

        /**
         * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
         * Top or Bottom, so we do it ourselves.
         */
        void ensureOnTopOrOnBottomForOpenbox(const Window window);

    private:
        /**
         * Openbox won't move an undecorated DOCK WINDOW type to
         * Top, so we do it ourselves.
         */
        void setWindowOnTopForOpenbox(const Window window);

        /**
         * Openbox won't move an undecorated DOCK WINDOW type to
         * Bottom, so we do it ourselves.
         */
        void setWindowOnBottomForOpenbox(const Window window);

        /**
         * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
         * Top, so we do it ourselves.
         */
        void ensureOnTopForOpenbox(const Window window);

        /**
         * Openbox won't enforce an undecorated DOCK WINDOW type to STAY ON
         * Bottom, so we do it ourselves.
         */
        void ensureOnBottomForOpenbox(const Window window);

        // Members.
};
