
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

    public:
        // General.
        static inline const QString GROUP_CONFIGURABLE = APP_NAME;
        static inline const QString GROUP_VOLATILE = "Volatile";

        // Configurable.
        static inline const QString AUTOHIDE_CONTROLS =
            "00 Auto hide Controls";
        static inline const QString AUTOHIDE_DELAY =
            "01 Auto hide Delay";

        static inline const QString PREFERRED_DESKTOP =
            "02 Preferred Desktop";
        static inline const QString ALLOW_DESKTOP_DRAG =
            "03 Allow Desktop Drag";

        static inline const QString ON_TOP_INSTEAD =
            "04 Stick to Top";

        static inline const QString DIVIDER_1 =
            "05";

        static inline const QString BACKGROUND_COLOR =
            "06 Background Color";
        static inline const QString BACKGROUND_OPACITY =
            "07 Background Opacity";

        static inline const QString MAX_STAR_SIZE =
            "08 Maximum Star Size";
        static inline const QString STAR_SATURATION =
            "09 Starfield Saturation";

        static inline const QString DIVIDER_2 =
            "10";

        static inline const QString SIZE_CHANGE_DELAY =
            "11 Size Change Delay";
        static inline const QString POSITION_CHANGE_DELAY =
            "12 Position Change Delay";
        static inline const QString COLOR_CHANGE_DELAY =
            "13 Color Change Delay";

        static inline const QString DIVIDER_3 =
            "14";

        static inline const QString STAR_COLOR_COOL =
            "15 Star Color Cool";
        static inline const QString STAR_COLOR_WARM =
            "16 Star Color Warm";
        static inline const QString STAR_COLOR_MEDIUM =
            "17 Star Color Medium";
        static inline const QString STAR_COLOR_HOT =
            "18 Star Color Hot";

        // Volatile.

        static inline const QString CONFIG_MODE = "In Config mode";

        // Settings property struct.
        struct SettingsProperty {
            QString group = "";
            QString name = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
            int rangeMinimum = numeric_limits<int>::min();
            int rangeMaximum = numeric_limits<int>::max();
        };

        static inline const vector<SettingsProperty> PROPERTIES = {
            // App configurables.
            { .group = GROUP_CONFIGURABLE, .name = AUTOHIDE_CONTROLS,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = AUTOHIDE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "4",
              .rangeMinimum = 1, .rangeMaximum = 9
            },

            { .group = GROUP_CONFIGURABLE, .name = PREFERRED_DESKTOP,
              .valueType = SLIDER_VALUETYPE, .initialValue = "-1",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = ALLOW_DESKTOP_DRAG,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = ON_TOP_INSTEAD,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = DIVIDER_1,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = BACKGROUND_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "#0055ff",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = BACKGROUND_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "50",
              .rangeMinimum = 0, .rangeMaximum = 255
            },

            { .group = GROUP_CONFIGURABLE, .name = MAX_STAR_SIZE,
              .valueType = SLIDER_VALUETYPE, .initialValue = "15",
              .rangeMinimum = 5, .rangeMaximum = 40
            },

            { .group = GROUP_CONFIGURABLE, .name = STAR_SATURATION,
              .valueType = SLIDER_VALUETYPE, .initialValue = "25",
              .rangeMinimum = 5, .rangeMaximum = 100
            },

            { .group = GROUP_CONFIGURABLE, .name = DIVIDER_2,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = COLOR_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .group = GROUP_CONFIGURABLE, .name = SIZE_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .group = GROUP_CONFIGURABLE, .name = POSITION_CHANGE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "500",
              .rangeMinimum = 1, .rangeMaximum = 2000
            },

            { .group = GROUP_CONFIGURABLE, .name = DIVIDER_3,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = STAR_COLOR_COOL,
              .valueType = COLOR_VALUETYPE, .initialValue = "#fff9d7",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = STAR_COLOR_WARM,
              .valueType = COLOR_VALUETYPE, .initialValue = "#FFBF00",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = STAR_COLOR_MEDIUM,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff7b08",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = STAR_COLOR_HOT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff1170",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            // Volatile internals, hidden from ConfigDialog.
            { .group = GROUP_VOLATILE, .name = CONFIG_MODE,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = 0, .rangeMaximum = 1
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
         * Helper to return a QSettings filename from appName.
         */
        QString getQSettingsFile();

        /**
         * Helper to return a new QSettings object for pref
         * access based on our appName.
         */
        QSettings* getQSettings();

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
         * Return the group of a Setting by key.
         */
        QString getSettingsGroup(const QString key);

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

    private:
        // Members.
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;
};