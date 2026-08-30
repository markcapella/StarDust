
#pragma once

#include <QDesktopServices>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

/**
 * Simple class to represent an AboutDialog.
 *
 */
class AboutDialog : public QDialog {
    Q_OBJECT

    public:
        static inline const int FONT_BASE_SIZE = 10;

        // Tiling Window managers.
        static inline const QStringList TILING_WM_NAMES = {
            "awesome", "i3", "dwm", "awesome", "bspwm", "xmonad",
            "herbstluftwm"
        };

        /**
         * Constructor.
         */
        explicit AboutDialog(QWidget* parent = nullptr);

    protected:
        /**
         * Close the AboutDialog when the window is closed
         * by clicking top-right 'X' button.
         *
         * Accept the close event so the system knows we handled
         * the window destruction.
         */
        void closeEvent(QCloseEvent* event) override;

    private:
        /**
         * Members.
         */
};
