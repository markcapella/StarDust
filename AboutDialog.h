
#pragma once

#include <QDesktopServices>
#include <QLabel>
#include <QMessageBox>


/**
 * Simple class to represent an AboutDialog.
 *
 */
class AboutDialog : public QDialog {
    Q_OBJECT

    public:
        // Tiling Window managers.
        static inline const QStringList TILING_WM_NAMES = {
            "awesome", "i3", "dwm", "awesome", "bspwm", "xmonad",
            "herbstluftwm"
        };

        explicit AboutDialog(QWidget* parent = nullptr);

    protected:
        void closeEvent(QCloseEvent* event) override;
};
