
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
        static inline const int CONFIG_DIALOG_WIDTH = 575;
        static inline const int CONFIG_DIALOG_HEIGHT = 720;

        static inline const int FORM_TOP_BOTTOM_SPACING = 15;
        static inline const int FORM_LAYOUT_ROW_SPACING = 8;

        // Constructor.
        explicit ConfigDialog(QWidget* parent = nullptr);

        /**
         * Translate Settings to desired language for display.
         */
        void translateConfigDialog();

        /**
         * Load UI form with values from .Ini.
         */
        void loadConfigDialog();

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
        void updateConfigDialog();

    private:
        Window mWindow = None;

        QFormLayout* mFormLayout = nullptr;
        QVBoxLayout* mMainLayout = nullptr;

        QDialogButtonBox* mConfigButtonBox = nullptr;

        QPushButton* mAboutButton = nullptr;
        QPushButton* mOkButton = nullptr;
        QPushButton* mApplyButton = nullptr;
        QPushButton* mCancelButton = nullptr;

        QList<bool> mSettingChanges;

        QDialog* mAboutDialog = nullptr;

        /**
         * Build the UI form layout.
         */
        void createConfigDialog();

        /**
         * Called on Ok button of Dialog clicked.
         */
        void okConfigDialog();

        /**
         * Called on Accept button of Dialog clicked.
         */
        void acceptConfigDialog();

        /**
         * Send an event to the X11 thread telling it to update
         * with new user config settings.
         */
        void sendConfigDialogUpdatedEvent(
            const bool canvasNeedsRedraw);

        /**
         * Show this apps "About" dialog.
         */
        void showAboutDialog();
};
