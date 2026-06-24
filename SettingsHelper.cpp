
#include "Global.h"

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 */
SettingsHelper::SettingsHelper() {
}

SettingsHelper::~SettingsHelper() {
    delete mQSettings;
}

/**
 * Getters for window minimum Width & Height.
 */
double
SettingsHelper::getWindowMinimumWidth() {
    return getQSettings()->value("minimumWindowWidth",
        Button::BUTTON_WIDTH * 5).toDouble();
}
double
SettingsHelper::getWindowMinimumHeight() {
    return getQSettings()->value("minimumWindowHeight",
        Button::BUTTON_HEIGHT * 5).toDouble();
}

/**
 * Setters for window minimum Width & Height.
 */
void
SettingsHelper::setWindowMinimumWidth(const double width) {
    const int WINDOW_MINIMUM_WIDTH = Button::BUTTON_WIDTH * 3;
    const int WIDTH = width < WINDOW_MINIMUM_WIDTH ?
        WINDOW_MINIMUM_WIDTH : width;
    return getQSettings()->setValue("minimumWindowWidth", WIDTH);
}
void
SettingsHelper::setWindowMinimumHeight(const double height) {
    const int WINDOW_MINIMUM_HEIGHT = Button::BUTTON_HEIGHT * 3;
    const int HEIGHT = height < WINDOW_MINIMUM_HEIGHT ?
        WINDOW_MINIMUM_HEIGHT : height;
    return getQSettings()->setValue("minimumWindowHeight", HEIGHT);
}

/**
 * Getters for window x / y, width / height.
 */
double
SettingsHelper::getWindowXPos() {
    return getQSettings()->value("windowXPos", -1).toDouble();
}
double
SettingsHelper::getWindowYPos() {
    return getQSettings()->value("windowYPos", -1).toDouble();
}
double
SettingsHelper::getWindowWidth() {
    return getQSettings()->value("windowWidth", -1).toDouble();
}
double
SettingsHelper::getWindowHeight() {
    return getQSettings()->value("windowHeight", -1).toDouble();
}

/**
 * Setters for window x / y, width / height.
 */
void
SettingsHelper::setWindowXPos(const double xPos) {
    if (xPos != getWindowXPos()) {
        getQSettings()->setValue("windowXPos", xPos);
    }
}
void
SettingsHelper::setWindowYPos(const double yPos) {
    if (yPos != getWindowYPos()) {
        getQSettings()->setValue("windowYPos", yPos);
    }
}
void
SettingsHelper::setWindowWidth(const double width) {
    if (width != getWindowWidth()) {
        getQSettings()->setValue("windowWidth", width);
    }
}
void
SettingsHelper::setWindowHeight(const double height) {
    if (height != getWindowHeight()) {
        getQSettings()->setValue("windowHeight", height);
    }
}

/**
 * Getters for canvas x & y, w & h.
 */
double
SettingsHelper::getCanvasXPos() {
    return getQSettings()->value("canvasXPos", -1).toDouble();
}
double
SettingsHelper::getCanvasYPos() {
    return getQSettings()->value("canvasYPos", -1).toDouble();
}
double
SettingsHelper::getCanvasWidth() {
    return getQSettings()->value("canvasWidth", 1000).toDouble();
}
double
SettingsHelper::getCanvasHeight() {
    return getQSettings()->value("canvasHeight", 600).toDouble();
}

/**
 * Setters for canvas x & y, w & h.
 */
void
SettingsHelper::setCanvasXPos(const double xPos) {
    if (xPos != getCanvasXPos()) {
        getQSettings()->setValue("canvasXPos", xPos);
    }
}
void
SettingsHelper::setCanvasYPos(const double yPos) {
    if (yPos != getCanvasYPos()) {
        getQSettings()->setValue("canvasYPos", yPos);
    }
}
void
SettingsHelper::setCanvasWidth(const double width) {
    if (width != getCanvasWidth()) {
        getQSettings()->setValue("canvasWidth", width);
    }
}
void
SettingsHelper::setCanvasHeight(const double height) {
    if (height != getCanvasHeight()) {
        getQSettings()->setValue("canvasHeight", height);
    }
}

/**
 * Helper to return a QSettings filename from appName.
 */
QString
SettingsHelper::getQSettingsFile() {
    return getenv("HOME") + QStringLiteral("/.local/") +
        QString(APP_NAME) + "/" + mRecentsHelper->
            getAppRecentsName() + QStringLiteral(".ini");
}

/**
 * Helper to return a new QSettings object for pref
 * access based on our appName.
 */
QSettings*
SettingsHelper::getQSettings() {
    if (!mQSettings) {
        mQSettings = new QSettings(getQSettingsFile(),
            QSettings::IniFormat);
    }
    return mQSettings;
}

/**
 * Each runtime start we ensure Settings keys & default values
 * are flushed to .ini file for ConfigDialog to load & modify.
 */
void
SettingsHelper::ensureSettingsAreConfigurable() {
    const int INITIAL_SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < INITIAL_SETTINGS_SIZE; i++) {
        const SettingsProperty property = PROPERTIES[i];
        getQSettings()->beginGroup(property.group);

        const QVariant SETTING_VARIANT =
            getQSettings()->value(property.name);
        if (!SETTING_VARIANT.isValid()) {
            getQSettings()->setValue(property.name, property.initialValue);
        }

        getQSettings()->endGroup();
    }

    mSettingsHelper->getQSettings()->sync();
}

/**
 * Getter for user configurable bool settings.
 */
bool
SettingsHelper::getBoolSetting(const QString setting) {
    getQSettings()->beginGroup(mSettingsHelper->
        getSettingsGroup(setting));

    const bool RESULT = getQSettings()->value(setting,
        getSettingsDefaultValue(setting)).toBool();

    getQSettings()->endGroup();

    return RESULT;
}

/**
 * Setter for user configurable bool settings.
 */
void
SettingsHelper::setBoolSetting(const QString setting,
    const bool value) {
    getQSettings()->beginGroup(mSettingsHelper->
        getSettingsGroup(setting));

    getQSettings()->setValue(setting, value);

    getQSettings()->endGroup();
}

/**
 * Getter for user configurable int settings.
 */
int
SettingsHelper::getIntSetting(const QString setting) {
    getQSettings()->beginGroup(mSettingsHelper->
        getSettingsGroup(setting));

    const int RESULT = getQSettings()->value(setting,
        getSettingsDefaultValue(setting)).toInt();

    getQSettings()->endGroup();

    return RESULT;
}

/**
 * Setter for user configurable int settings.
 */
void
SettingsHelper::setIntSetting(const QString setting,
    int value) {
    getQSettings()->beginGroup(mSettingsHelper->
        getSettingsGroup(setting));

    getQSettings()->setValue(setting, value);

    getQSettings()->endGroup();
}

/**
 * Getter for user configurable XRenderColor settings.
 */
XRenderColor
SettingsHelper::getColorSetting(const QString setting) {
    getQSettings()->beginGroup(mSettingsHelper->
        getSettingsGroup(setting));

    const QColor COLOR = QColor(getQSettings()->value(
        setting, getSettingsDefaultValue(setting)).toString());

    getQSettings()->endGroup();

    XRenderColor xColor;
    xColor.red   = (COLOR.red() << 8) | 0xff;
    xColor.green = (COLOR.green() << 8) | 0xff;
    xColor.blue  = (COLOR.blue() << 8) | 0xff;
    xColor.alpha = (COLOR.alpha() << 8) | 0xff;
    return xColor;
}

/**
 * Return the group of a Setting by key.
 */
QString
SettingsHelper::getSettingsGroup(const QString key) {
    const int SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsProperty THIS_SETTING = PROPERTIES[i];
        if (key == THIS_SETTING.name) {
            return THIS_SETTING.group;
        }
    }
    return "";
}

/**
 * Return the value type of a Setting by key.
 */
SettingsPropertyType
SettingsHelper::getSettingsValueType(const QString key) {
    const int SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsProperty THIS_SETTING = PROPERTIES[i];
        if (key == THIS_SETTING.name) {
            return THIS_SETTING.valueType;
        }
    }
    return NONE_VALUETYPE;
}

/**
 * Return the default value of a Setting by key.
 */
QString
SettingsHelper::getSettingsDefaultValue(const QString key) {
    QString resultValue = "";
    const int SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsProperty THIS_SETTING = PROPERTIES[i];
        if (key == THIS_SETTING.name) {
            resultValue = THIS_SETTING.initialValue;
            break;
        }
    }
    return resultValue;
}

/**
 * Get a Minimum int value to load a UI widget.
 */
int
SettingsHelper::getSettingsIntRangeMinimum(const QString key) {
    if (key == PREFERRED_DESKTOP) {
       return -1;
    }

    int resultValue = numeric_limits<int>::min();
    const int SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsProperty THIS_SETTING = PROPERTIES[i];
        if (key == THIS_SETTING.name) {
            resultValue = THIS_SETTING.rangeMinimum;
            break;
        }
    }
    return resultValue;
}

/**
 * Get a Maximum int value to load a UI widget.
 */
int
SettingsHelper::getSettingsIntRangeMaximum(const QString key) {
    if (key == PREFERRED_DESKTOP) {
        return mXHelper->getMaximumDesktops() - 1;
    }

    int resultValue = numeric_limits<int>::max();
    const int SETTINGS_SIZE = PROPERTIES.size();

    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsProperty THIS_SETTING = PROPERTIES[i];
        if (key == THIS_SETTING.name) {
            resultValue = THIS_SETTING.rangeMaximum;
            break;
        }
    }
    return resultValue;
}
