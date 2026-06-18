
#pragma once

/**
 * RecentsHelper provides common related methods.
 */
class RecentsHelper {

    public:
        struct ProcessInfo {
            bool isPidValid = false;
            int pid = -1;
        };

        // Tiling Window managers.
        static inline const QStringList RECENTS_NAMES = {
            "one", "two", "three", "four",
            "five", "six", "seven", "eight" };

        /**
         * Constructor.
         */
        RecentsHelper();

        /**
         * Helper method to determine the apps recents name.
         */
        QString getAppRecentsName();

    private:
        // Members.
        QString mRecentsName = "";

        struct flock mRecentsLock;

        /**
         * Helper to return the path of this apps lock file.
         */
        QString getAppLockPathName();

        /**
         * Helper to return the name of this apps lock file.
         */
        QString getAppLockFileName();

        /**
         * Locks the apps lock file while we
         * determine this app-instance recents name.
         */
        int lockRecentsProcessInfo();

        /**
         * Determine this app-instance recents name.
         */
        QString getRecentsName();

        /**
         * For all procs named APP_NAME, get their window titles
         * which expose their recents name.
         */
        vector<string> getActiveProcWindowTitles();

        /**
         * Scan all window titles of our active instances;
         * Determine proc recents name from window title;
         * Determine first recents not in use, assign & return;
         */
        vector<QString> getActiveRecentNamesFromProcWindowTitles(
            const vector<string> procTitles);

        /**
         * Scan the list of active recents to see if there are
         * recents not currently active whose id we can assume.
         */
        QString getRecentsNameNotActive(const vector<QString> recentNames);

        /**
         * If there are no recent names available in the list,
         * add one from end, or return "" if full.
         */
        QString getNextAvailableRecentsNameOrNone(
            const vector<QString> recentNames);

        /**
         * Unlocks the apps lock file.
         */
        void unlockRecentsProcessInfo(const int fd);
};
