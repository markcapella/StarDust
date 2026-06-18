
#include "Global.h"

/**
 * RecentsHelper provides common related methods.
 */
RecentsHelper::RecentsHelper() {
}

/**
 * Helper method to determine the apps recents name.
 */
QString
RecentsHelper::getAppRecentsName() {
    // Return value if already set.
    if (!mRecentsName.isEmpty()) {
        return mRecentsName;
    }

    int mRecentsFD = lockRecentsProcessInfo();
    if (mRecentsFD == -1) {
        cout << endl << XCOLOR_RED << "Uunable to start due to "
            "control file contention. Please try again later." << endl;
        QMessageBox::information(NULL, APP_NAME, "Unable to start due "
            "to control file contention. Please try again later.");
        return "";
    }

    mRecentsName = getRecentsName();
    unlockRecentsProcessInfo(mRecentsFD);

    if (mRecentsName.isEmpty()) {
        QMessageBox::information(NULL, APP_NAME, "Running the maximum "
            "amount of instances already, ( " + mRecentsHelper->
            RECENTS_NAMES[mRecentsHelper->RECENTS_NAMES.size() - 1] +
            " ).");
        cout << endl << XCOLOR_RED << "Running the maximum amount of "
            "instances already, ( " << mRecentsHelper->
            RECENTS_NAMES[mRecentsHelper->RECENTS_NAMES.size() - 1].
            toStdString() << " )." << endl;
        return "";
    }

    close(mRecentsFD);
    return mRecentsName;
}

/**
 * Helper to return the path of this apps lock file.
 */
QString
RecentsHelper::getAppLockPathName() {
    return getenv("HOME") + QStringLiteral("/.local/") +
        QString(APP_NAME) + QStringLiteral("/");
}

/**
 * Helper to return the name of this apps lock file.
 */
QString
RecentsHelper::getAppLockFileName() {
    return getAppLockPathName() + QStringLiteral("lockFile");
}

/**
 * Locks the apps lock file while we
 * determine this app-instance recents name.
 */
int
RecentsHelper::lockRecentsProcessInfo() {
    // Create .local folder path if not exist.
    QFileInfo fileInfo(getAppLockPathName());
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            cout << endl << XCOLOR_RED << "Cannot create local "
                "folder: " << dir.path().toStdString() << "." << endl;
            return -1;
        }
    }

    // Create lock file if not exist.
    const QString RECENTS_FILE = getAppLockFileName();
    char* recentsFile = strdup(RECENTS_FILE.toUtf8().constData());

    // Open lock file
    int fdLocked = open(recentsFile, O_CREAT | O_RDWR, 0644);
    if (fdLocked == -1) {
        free(recentsFile);
        cout << endl << "FAIL " << RECENTS_FILE.toStdString() <<
            "." << endl;
        return -1;
    }

    bool locked = false;
    chrono::steady_clock::time_point startTime =
        chrono::steady_clock::now();

    mRecentsLock.l_type = F_WRLCK;
    mRecentsLock.l_whence = SEEK_SET;
    mRecentsLock.l_start = 0;
    mRecentsLock.l_len = 0;

    const int WAIT_TIME = 5;
    while (chrono::steady_clock::now() - startTime <
        chrono::seconds(WAIT_TIME)) {
        if (fcntl(fdLocked, F_SETLK, &mRecentsLock) == 0) {
            locked = true;
            break;
        }
        usleep(50000); // 50ms.
    }
    free(recentsFile);

    // Pass back the fdLocked if its locked & ours.
    if (locked) {
        return fdLocked;
    }

    // Close it, can't use it.
    close(fdLocked);
    return -1;
}

/**
 * Determine this app-instance recents name.
 */
QString
RecentsHelper::getRecentsName() {
    const vector<string> ACTIVE_TITLES =
        getActiveProcWindowTitles();

    const vector<QString> RECENT_NAMES =
        getActiveRecentNamesFromProcWindowTitles(ACTIVE_TITLES);

    const QString RECENT_NAME =
        getRecentsNameNotActive(RECENT_NAMES);

    if (!RECENT_NAME.isEmpty()) {
        return RECENT_NAME;
    }
    return getNextAvailableRecentsNameOrNone(RECENT_NAMES);
}

/**
 * For all procs named APP_NAME, get their window titles
 * which expose their recents name.
 */
vector<string>
RecentsHelper::getActiveProcWindowTitles() {
    vector<string> procTitles;

    DIR* procDir = opendir("/proc");
    if (!procDir) {
        return procTitles;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        // Only look at numeric directories (PIDs).
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            const string procPid = entry->d_name;

            ifstream comm("/proc/" + procPid + "/comm");
            string procName;
            if (comm.is_open()) {
                getline(comm, procName);
            }
            if (procName != APP_NAME) {
                continue;
            }

            try {
                const pid_t PID = static_cast<pid_t>(stoi(procPid));
                const string TITLE = mXHelper->getWindowTitleFromPID(PID);
                if (TITLE != "") {
                    procTitles.push_back(TITLE);
                }
            } catch (...){}
        }
    }

    closedir(procDir);
    return procTitles;
}

/**
 * Scan all window titles of our active instances;
 * Determine proc recents name from window title;
 * Determine first recents not in use, assign & return;
 */
vector<QString>
RecentsHelper::getActiveRecentNamesFromProcWindowTitles(
    const vector<string> procTitles) {

    vector<QString> activeRecents;

    const int TITLES_SIZE = procTitles.size();
    for (int i = 0; i < TITLES_SIZE; i++) {
        // Recents portion of title starts past App Name + " ";
        const int RECENTS_NAME_INDEX = QString(APP_NAME).length() + 1;

        const QString RECENT_NAME =
            QString::fromStdString(procTitles[i].
                substr(RECENTS_NAME_INDEX)).
                    trimmed();

        // We omit "one" from Recent name.
        if (RECENT_NAME.isEmpty()) {
            activeRecents.push_back(RecentsHelper::RECENTS_NAMES[0]);
        } else {
            activeRecents.push_back(RECENT_NAME);
        }
    }

    return activeRecents;
}

/**
 * Scan the list of active recents to see if there are
 * recents not currently active whose id we can assume.
 */
QString
RecentsHelper::getRecentsNameNotActive(
    const vector<QString> recentNames) {

    QStringList myList(recentNames.begin(), recentNames.end());

    const int RECENTS_SIZE = RecentsHelper::RECENTS_NAMES.size();
    for (int i = 0; i < RECENTS_SIZE; i++) {
        if (!myList.contains(RecentsHelper::RECENTS_NAMES[i])) {
            return RecentsHelper::RECENTS_NAMES[i];
        }
    }

    return "";
}

/**
 * If there are no recent names available in the list,
 * add one from end, or return "" if full.
 */
QString
RecentsHelper::getNextAvailableRecentsNameOrNone(
    const vector<QString> recentNames) {

    const int RECENTS_SIZE = RecentsHelper::RECENTS_NAMES.size();
    const int ACTIVE_RECENTS_SIZE = recentNames.size();

    return ACTIVE_RECENTS_SIZE < RECENTS_SIZE ?
        RecentsHelper::RECENTS_NAMES[ACTIVE_RECENTS_SIZE + 1] : "";
}

/**
 * Unlocks the apps lock file.
 */
void
RecentsHelper::unlockRecentsProcessInfo(const int fd) {
    mRecentsLock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &mRecentsLock);
}
