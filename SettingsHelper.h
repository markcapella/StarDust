
#pragma once

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 */
enum SettingsPropertyType {
    NONE_VALUETYPE,
    STRING_VALUETYPE,
    INT_VALUETYPE,
    BOOL_VALUETYPE,
    COLOR_VALUETYPE,
    SLIDER_VALUETYPE,
    DIVIDER_VALUETYPE
};

class SettingsHelper {
    #define IC_QString static inline const QString

    public:
        // Configurable.
        IC_QString AUTOHIDE_CONTROLS = "Auto hide Controls";
        IC_QString AUTOHIDE_DELAY = "Auto hide Delay";
        IC_QString PREFERRED_DESKTOP = "Preferred Desktop";
        IC_QString ALLOW_DESKTOP_DRAG = "Allow Desktop Drag";
        IC_QString ON_TOP_INSTEAD = "Stick to Top";
        IC_QString DIVIDER_1 = "divider01";

        IC_QString BACKGROUND_COLOR = "Background Color";
        IC_QString BACKGROUND_OPACITY = "Background Opacity";
        IC_QString MAX_STAR_SIZE = "Maximum Star Size";
        IC_QString STAR_SATURATION = "Starfield Saturation";
        IC_QString DIVIDER_2 = "divider02";

        IC_QString SIZE_CHANGE_DELAY = "Size Change Delay";
        IC_QString POSITION_CHANGE_DELAY = "Position Change Delay";
        IC_QString COLOR_CHANGE_DELAY = "Color Change Delay";
        IC_QString DIVIDER_3 = "divider03";

        IC_QString STAR_COLOR_COOL = "Star Color Cool";
        IC_QString STAR_COLOR_WARM = "Star Color Warm";
        IC_QString STAR_COLOR_MEDIUM = "Star Color Medium";
        IC_QString STAR_COLOR_HOT = "Star Color Hot";


        // Settings property struct.
        struct SettingsProperty {
            QString name = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
            int rangeMinimum = numeric_limits<int>::min();
            int rangeMaximum = numeric_limits<int>::max();
        };

        static inline const vector<SettingsProperty> PROPERTIES = {
            // App configurables.
            { .name = AUTOHIDE_CONTROLS,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = AUTOHIDE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "4",
              .rangeMinimum = 1, .rangeMaximum = 9
            },

            { .name = PREFERRED_DESKTOP,
              .valueType = SLIDER_VALUETYPE, .initialValue = "-1",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = ALLOW_DESKTOP_DRAG,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = ON_TOP_INSTEAD,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = DIVIDER_1,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "10",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = BACKGROUND_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "#0055ff",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = BACKGROUND_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "50",
              .rangeMinimum = 0, .rangeMaximum = 255
            },

            { .name = MAX_STAR_SIZE,
              .valueType = SLIDER_VALUETYPE, .initialValue = "15",
              .rangeMinimum = 5, .rangeMaximum = 40
            },

            { .name = STAR_SATURATION,
              .valueType = SLIDER_VALUETYPE, .initialValue = "25",
              .rangeMinimum = 5, .rangeMaximum = 100
            },

            { .name = DIVIDER_2,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = COLOR_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .name = SIZE_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .name = POSITION_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .name = DIVIDER_3,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = STAR_COLOR_COOL,
              .valueType = COLOR_VALUETYPE, .initialValue = "#fff9d7",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = STAR_COLOR_WARM,
              .valueType = COLOR_VALUETYPE, .initialValue = "#FFBF00",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = STAR_COLOR_MEDIUM,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff7b08",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = STAR_COLOR_HOT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff1170",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            }
        };

        SettingsHelper();
        ~SettingsHelper();

        /**
         * Getters for window minimum Width & Height.
         */
        double getWindowMinimumWidth();
        double getWindowMinimumHeight();

        /**
         * Setters for window minimum Width & Height.
         */
        void setWindowMinimumWidth(const double width);
        void setWindowMinimumHeight(const double height);

        /**
         * Getters for window x & y, w & h.
         */
        double getWindowXPos();
        double getWindowYPos();
        double getWindowWidth();
        double getWindowHeight();

        /**
         * Setters for window x & y, w & h.
         */
        void setWindowXPos(const double xPos);
        void setWindowYPos(const double yPos);
        void setWindowWidth(const double width);
        void setWindowHeight(const double height);

        /**
         * Getters for canvas x & y, w & h.
         */
        double getCanvasXPos();
        double getCanvasYPos();
        double getCanvasWidth();
        double getCanvasHeight();

        /**
         * Setters for canvas x & y, w & h.
         */
        void setCanvasXPos(const double xPos);
        void setCanvasYPos(const double yPos);
        void setCanvasWidth(const double width);
        void setCanvasHeight(const double height);

        /**
         * Getters & setters of window config mode.
         */
        bool getConfigMode();

        void setConfigMode(const bool state);

        /**
         * Getters & setters for user configurable bool settings.
         */
        bool getBoolSetting(const QString setting);

        void setBoolSetting(const QString setting, const bool value);

        /**
         * Getter for user configurable int settings.
         */
        int getIntSetting(const QString setting);

        /**
         * Setter for user configurable int settings.
         */
        void setIntSetting(const QString setting, int value);

        /**
         * Getters & setters for user configurable XRenderColor settings.
         */
        XRenderColor getColorSetting(const QString setting);

        /**
         * Each runtime start we ensure Settings keys & default values
         * are flushed to .ini file for ConfigDialog to load & modify.
         */
        void ensureSettingsAreConfigurable();

        /**
         * Return the value type of a Setting by key.
         */
        SettingsPropertyType getSettingsValueType(const QString key);

        /**
         * Return the default value of a Setting by key.
         */
        QString getSettingsDefaultValue(const QString key);

        /**
         * Get a Minimum int value to load a UI widget.
         */
        int getSettingsIntRangeMinimum(const QString key);

        /**
         * Get a Maximum int value to load a UI widget.
         */
        int getSettingsIntRangeMaximum(const QString key);

        /**
         * Helper to return a new QSettings object for pref
         * access based on our appName.
         */
        QSettings* getQSettings();

    private:
        // Members.
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;

        /**
         * Helper to return a QSettings filename from appName.
         */
        QString getQSettingsFile();
};