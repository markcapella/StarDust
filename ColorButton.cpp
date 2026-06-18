
#include "Global.h"

/**
 * Simple class to represent a ColorButton.
 *
 */
ColorButton::ColorButton(const QString title, QWidget* parent) :
    QPushButton(parent), mTitle(title), mColor(Qt::white) {

//    mTitle = title;

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    updateButtonStyle();

    connect(this, &QPushButton::clicked,
        this, &ColorButton::openColorPicker);
}

/**
 * Getter for current color.
 */
QColor
ColorButton::getButtonColor() const {
    return mColor;
}

/**
 * Open & show ColorPicker Dialog.
 */
void
ColorButton::openColorPicker() {
    QColorDialog* COLOR_PICKER = new QColorDialog(mColor, this);

    COLOR_PICKER->setWindowTitle("Select " + mTitle);
    COLOR_PICKER->setOption(QColorDialog::ShowAlphaChannel);
    COLOR_PICKER->setAttribute(Qt::WA_DeleteOnClose);

    connect(COLOR_PICKER, &QColorDialog::colorSelected, this,
        &ColorButton::setButtonColor);

    COLOR_PICKER->open(); 
}

/**
 * Callback from ColorPicker.
 */
void
ColorButton::setButtonColor(const QColor& color) {
    if (color.isValid()) {
        //if (mColor != color) {
            mColor = color;
            updateButtonStyle();
            // emit colorChanged(mColor);
        //}
    }
}

/**
 * Set base ColorButton to color selected in
 * ColorPicker.
 */
void
ColorButton::updateButtonStyle() {
    setObjectName("ColorButton");

    const QString CSS = QString("ColorButton#%1 { "
        "margin-left: 1px; background-color: %2; "
        "border: 1px solid #555; border-radius: 4px; "
        "width: 15px; height: 15px;"
        "max-width: 15px; max-height: 15px; }").
        arg(objectName()).arg(mColor.name(QColor::HexArgb));

    setStyleSheet(CSS + " ColorButton { color: white; }");
}
