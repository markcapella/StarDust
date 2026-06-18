
#pragma once

#include <QColor>
#include <QColorDialog>
#include <QPushButton>


/**
 * Simple class to represent a ColorButton
 */
class ColorButton : public QPushButton {
    Q_OBJECT

    public:
        explicit ColorButton(const QString title,
            QWidget* parent = nullptr);

        /**
         * Getter for current color.
         */
        QColor getButtonColor() const;

        /**
         * Open & show ColorPicker Dialog.
         */
        void openColorPicker();

        /**
         * Callback from ColorPicker.
         */
        void setButtonColor(const QColor& color);

    private:
        QString mTitle;

        QColor mColor;

        /**
         * Set base ColorButton to color selected in
         * ColorPicker.
         */
        void updateButtonStyle();
};
