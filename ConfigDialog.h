
#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

#include <X11/Xutil.h>

/**
 * Simple class to represent a ConfigDialog.
 */
class ConfigDialog : public QDialog {
    Q_OBJECT

    public:
        static inline const int CONFIG_DIALOG_WIDTH = 450;
        static inline const int CONFIG_DIALOG_HEIGHT = 600;

        static inline const int FORM_TOP_BOTTOM_SPACING = 15;
        static inline const int FORM_LAYOUT_ROW_SPACING = 8;

        static inline const QString ABOUT_STRING = "About";

        // Constructor.
        explicit ConfigDialog(QWidget* parent = nullptr);

        /**
         * Load UI form with values from .Ini.
         */
        void loadConfigDialogControls();

        /**
         * Gettters / Setters for window.
         */
        Window getWindow() const {
            return mWindow;
        }
        void setWindow(const Window window) {
            mWindow = window;
        }

        /**
         * Update any runtime dialog controls, range settings, etc.
         */
        void updateConfigDialogControls();

    private:
        Window mWindow = None;
        Atom mConfigDialogUpdated{};

        QFormLayout* mFormLayout = nullptr;
        QVBoxLayout* mMainLayout = nullptr;

        QDialogButtonBox* mConfigButtonBox = nullptr;
        QPushButton* mAboutButton = nullptr;
        QDialog* mAboutDialog = nullptr;

        /**
         * Build the UI form layout.
         */
        void createConfigDialogControls();

        /**
         * Callback to Save UI form values to .Ini.
         */
        void acceptConfigDialogControls();

        /**
         * Send an event to the X11 thread telling it to update
         * with new user config settings.
         */
        void sendConfigDialogUpdatedEvent();

        /**
         * Show this apps "About" dialog.
         */
        void about();
};
