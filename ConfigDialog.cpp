
#include "Global.h"

#include <QStyleFactory>

/**
 * Simple class to represent a ConfigDialog.
 */
ConfigDialog::ConfigDialog(QWidget* parent) : QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::Tool);
    resize(CONFIG_DIALOG_WIDTH, CONFIG_DIALOG_HEIGHT);
    setFixedSize(size());

    // Set the window title.
    const QString FIRST_RECENTS_NAME = mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME = mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + I18N(APP_RECENT_NAME);
    }
    TITLE += " " + I18N("Settings");
    setWindowTitle(QString(TITLE));

    // Set the window attributes, create controls & center.
    createConfigDialog();
    mFormLayout->setFormAlignment(Qt::AlignCenter);

    // Add the formlayout to a formcontainer.
    QWidget* formContainer = new QWidget();
    formContainer->setLayout(mFormLayout);

    // Add the formcontainer to a scrollarea.
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(formContainer);
    scrollArea->setWidgetResizable(true);

    // The whole thing wraps up into vbox layout.
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->addWidget(scrollArea);

    // Create Buttons Layout.
    QHBoxLayout* mButtonLayout = new QHBoxLayout();

    // Create all buttons for the layout.
    mAboutButton = new QPushButton(I18N("About"), this);
    mOkButton = new QPushButton(I18N("Ok"), this);
    mApplyButton = new QPushButton(I18N("Apply"), this);
    mCancelButton = new QPushButton(I18N("Cancel"), this);

    // Ensure nothing defaults to having focus.
    mAboutButton->setAutoDefault(false);
    mOkButton->setAutoDefault(false);
    mApplyButton->setAutoDefault(false);
    mCancelButton->setAutoDefault(false);

    // Add all buttons to the layout.
    mButtonLayout->addWidget(mAboutButton);
    mButtonLayout->addStretch();
    mButtonLayout->addWidget(mOkButton);
    mButtonLayout->addWidget(mApplyButton);
    mButtonLayout->addWidget(mCancelButton);

    // Add buttons widget to layout & set as Layout.
    mMainLayout->addLayout(mButtonLayout);
    setLayout(mMainLayout);

    // Connect all button click signals.
    connect(mAboutButton, &QPushButton::clicked, this,
        &ConfigDialog::showAboutDialog);
    connect(mOkButton, &QPushButton::clicked, this,
        &ConfigDialog::okConfigDialog);
    connect(mApplyButton, &QPushButton::clicked, this,
        &ConfigDialog::acceptConfigDialog);
    connect(mCancelButton, &QPushButton::clicked, this,
        &ConfigDialog::reject);

    // Init settings change list, size / value.
    mSettingChanges.fill(false, SettingsHelper::PROPERTIES.size());
}

/**
 * Translate Settings to desired language for display.
 */
void
ConfigDialog::translateConfigDialog() {
    // Set the window title.
    const QString FIRST_RECENTS_NAME = mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME = mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + I18N(APP_RECENT_NAME);
    }
    TITLE += " " + I18N("Settings");
    setWindowTitle(QString(TITLE));

    const int FORM_LAYOUT_SIZE = mFormLayout->rowCount();
    for (int i = 0; i < FORM_LAYOUT_SIZE; ++i) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];

        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;

        // Ignore Divider lines.
        if (THIS_VALUETYPE == DIVIDER_VALUETYPE) {
            continue;
        }

        const QLayoutItem* ROW = mFormLayout->itemAt(
            i, QFormLayout::LabelRole);
        if (ROW) {
            QLabel* label = qobject_cast<QLabel*>(ROW->widget());
            if (label) {
                const QString VALUE = I18N(THIS_KEY);
                label->setText(VALUE);
            }
        }
    }

    mAboutButton->setText(I18N("About"));
    mOkButton->setText(I18N("Ok"));
    mApplyButton->setText(I18N("Apply"));
    mCancelButton->setText(I18N("Cancel"));

    mAboutButton->clearFocus();
    mOkButton->clearFocus();
    mApplyButton->clearFocus();
    mCancelButton->clearFocus();
}

/**
 * Load UI form with values from .Ini.
 */
void
ConfigDialog::loadConfigDialog() {

    const int FORM_LAYOUT_SIZE = mFormLayout->rowCount();
    for (int i = 0; i < FORM_LAYOUT_SIZE; ++i) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];
        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;
        const QString THIS_DEFAULT_VALUE = THIS_SETTING.initialValue;

        // Ignore Divider lines.
        if (THIS_VALUETYPE == DIVIDER_VALUETYPE) {
            continue;
        }

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                stringEditWidget->setText(VALUE);
            }
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                const int VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toInt();
                lineEditWidget->setText(QString::number(VALUE));
            }
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                checkboxWidget->setCheckState(VALUE == "true" ?
                    Qt::Checked : Qt::Unchecked );
            }
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                colorButtonWidget->setButtonColor(QColor(VALUE));
            }
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (sliderEditWidget) {
                sliderEditWidget->setMinimum(mSettingsHelper->
                    getSettingsIntRangeMinimum(THIS_KEY));
                sliderEditWidget->setMaximum(mSettingsHelper->
                    getSettingsIntRangeMaximum(THIS_KEY));
                    const int VALUE = mSettingsHelper->getIntSetting(
                        THIS_KEY);
                    sliderEditWidget->setSliderPosition(VALUE);
            }
            continue;
        }

        // Get QComboBox for Language.
        if (THIS_VALUETYPE == COMBOBOX_VALUETYPE &&
            THIS_KEY == SettingsHelper::APP_LANGUAGE) {
            QComboBox* langComboWidget = nullptr;
            langComboWidget = qobject_cast<QComboBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (langComboWidget) {
                const QString LANG = mSettingsHelper->
                    getStringSetting(SettingsHelper::APP_LANGUAGE);
                const int LANG_INDEX = ALL_LANGUAGES.indexOf(LANG);
                langComboWidget->setCurrentIndex(LANG_INDEX);
            }
            continue;
        }
    }

    // Init Apply button after load.
    mSettingChanges.fill(false);
    mApplyButton->setEnabled(false);
}

/**
 * Update any runtime dialog controls, range settings, etc.
 */
void
ConfigDialog::updateConfigDialog() {

    const int FORM_LAYOUT_SIZE = mFormLayout->rowCount();
    for (int i = 0; i < FORM_LAYOUT_SIZE; ++i) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];
        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;

        // Reset desktop preference slider for 2 reasons.
        if (THIS_KEY == SettingsHelper::PREFERRED_DESKTOP) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (sliderEditWidget) {
                // Reset preferred desktop slider range maximum
                // as OS can change max desktops while dialog open.
                const int CURRENT_MAX = sliderEditWidget->maximum();
                const int ACTUAL_MAX = mXHelper->getMaximumDesktops() - 1;
                if (CURRENT_MAX != ACTUAL_MAX) {
                    sliderEditWidget->setMaximum(ACTUAL_MAX);
                }

                // Reset current desktop slider value as window drag
                // can change preferred desktop while dialog open.
                const int SLIDER_CURRENT = sliderEditWidget->
                    sliderPosition();
                const int VALUE_CURRENT = mSettingsHelper->getIntSetting(
                    mSettingsHelper->SettingsHelper::PREFERRED_DESKTOP);
                if (SLIDER_CURRENT != VALUE_CURRENT) {
                    sliderEditWidget->setSliderPosition(VALUE_CURRENT);
                    mSettingChanges[i] = false;
                    const bool APPLY_BUTTON_ENABLED =
                        mSettingChanges.contains(true);
                    mApplyButton->setEnabled(APPLY_BUTTON_ENABLED);
                }
            }
        }
    }
}

/**
 * Build the UI form layout.
 */
void
ConfigDialog::createConfigDialog() {
    // Build form.
    mFormLayout = new QFormLayout();
    mFormLayout->setContentsMargins(0, FORM_TOP_BOTTOM_SPACING,
        0, FORM_TOP_BOTTOM_SPACING);
    mFormLayout->setVerticalSpacing(FORM_LAYOUT_ROW_SPACING);

    const int SETTINGS_SIZE = SettingsHelper::PROPERTIES.size();
    for (int i = 0; i < SETTINGS_SIZE; i++) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];
        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE = THIS_SETTING.valueType;
        const QString I18N_DISPLAY_KEY = I18N(THIS_KEY);

        // Get QLineEdit for Divider lines.
        if (THIS_VALUETYPE == DIVIDER_VALUETYPE) {
            QLabel* dividerWidget = new QLabel(this);
            dividerWidget->setObjectName(THIS_KEY);
            const int SLIDER_HEIGHT_VALUE = mSettingsHelper->
                getIntSetting(THIS_KEY);
            dividerWidget->setFixedHeight(SLIDER_HEIGHT_VALUE);
            mFormLayout->addRow("", dividerWidget);
            continue;
        }

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = new QLineEdit(this);
            stringEditWidget->setObjectName(THIS_KEY);
            stringEditWidget->setFixedWidth(360);
            mFormLayout->addRow(I18N_DISPLAY_KEY, stringEditWidget);
            connect(stringEditWidget, &QLineEdit::textChanged,
                this, [this, i] (const QString &text) {
                mSettingChanges[i] = true;
                mApplyButton->setEnabled(true);
            });
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = new QLineEdit(this);
            lineEditWidget->setObjectName(THIS_KEY);
            lineEditWidget->setFixedWidth(120);
            mFormLayout->addRow(I18N_DISPLAY_KEY, lineEditWidget);
            connect(lineEditWidget, &QLineEdit::textChanged,
                this, [this, i] (const QString &text) {
                mSettingChanges[i] = true;
                mApplyButton->setEnabled(true);
            });
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = new QCheckBox(this);
            checkboxWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(I18N_DISPLAY_KEY, checkboxWidget);
            connect(checkboxWidget, &QCheckBox::toggled,
                this, [this, i] (bool checked) {
                mSettingChanges[i] = true;
                mApplyButton->setEnabled(true);
            });
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = new ColorButton(
                THIS_KEY, this);
            colorButtonWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(I18N_DISPLAY_KEY, colorButtonWidget);
            connect(colorButtonWidget, &ColorButton::colorChanged,
                this, [this, i] (const QColor &color) {
                mSettingChanges[i] = true;
                mApplyButton->setEnabled(true);
            });
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = new QSlider(Qt::Horizontal, this);
            sliderEditWidget->setObjectName(THIS_KEY);
            sliderEditWidget->setFixedWidth(160);
            mFormLayout->addRow(I18N_DISPLAY_KEY, sliderEditWidget);

            // Nice tooltip on slow hover.
            if (THIS_KEY == SettingsHelper::AUTOHIDE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = QString::number(value) +
                        " " + I18N("seconds");
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new AutoHideDelayHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::PREFERRED_DESKTOP) {
                // Sliders with tick marks get Fusion stlye.
                sliderEditWidget->setTickInterval(1);
                sliderEditWidget->setTickPosition(QSlider::TicksBelow);
                sliderEditWidget->setStyle(QStyleFactory::create("Fusion"));
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    // Use the calculated center of the slider for
                    // ToolTip, as updates happen during window drag and
                    // pointer is nowhere near the slider at that point.
                    const QPoint LOCAL_POSITION = sliderEditWidget->
                        rect().center(); 
                    const QPoint GLOBAL_POSITION = sliderEditWidget->
                        mapToGlobal(LOCAL_POSITION);
                    const QString TOOLTIP_TEXT = (value == -1) ?
                        I18N("All") : I18N("Desktop") + " " +
                        QString::number(value + 1);
                    QToolTip::showText(GLOBAL_POSITION, TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new DesktopPreferenceHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::BACKGROUND_OPACITY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const int VALUE_PCT = 100 * value / 255;
                    const QString TOOLTIP_TEXT =
                        QString::number(VALUE_PCT) + "%";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new OpacityHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::STAR_SATURATION) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value) + "%";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new SaturationHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::MAX_STAR_SIZE) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = QString::number(value) +
                        " " + I18N("pixels");
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new MaxStarSizeHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::COLOR_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new ColorChangeRateHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::SIZE_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new SizeChangeRateHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::POSITION_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    this, [this, i, sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                    mSettingChanges[i] = true;
                    mApplyButton->setEnabled(true);
                });
                sliderEditWidget->installEventFilter(
                    new PositionChangeRateHints(sliderEditWidget));
                continue;
            }
        }

        // Get QComboBox for Language.
        if (THIS_VALUETYPE == COMBOBOX_VALUETYPE &&
            THIS_KEY == SettingsHelper::APP_LANGUAGE) {
            QComboBox* langComboWidget = new QComboBox(this);
            langComboWidget->setItemDelegate(new ComboboxDelegate(
                langComboWidget));
            langComboWidget->addItems(ALL_LANGUAGES);
            langComboWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(I18N_DISPLAY_KEY, langComboWidget);
            connect(langComboWidget,&QComboBox::currentIndexChanged,
                this, [this, i] (int index) {
                mSettingChanges[i] = true;
                mApplyButton->setEnabled(true);
            });
            continue;
        }
    }
}

/**
 * Called on Ok button of Dialog clicked.
 */
void
ConfigDialog::okConfigDialog() {

    acceptConfigDialog();
    accept();
}

/**
 * Called on Accept button of Dialog clicked.
 */
void
ConfigDialog::acceptConfigDialog() {
    if (!mSettingChanges.contains(true)) {
        return;
    }

    const int FORM_LAYOUT_SIZE = mFormLayout->rowCount();
    for (int i = 0; i < FORM_LAYOUT_SIZE; ++i) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];
        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;

        // Ignore Divider lines.
        if (THIS_VALUETYPE == DIVIDER_VALUETYPE) {
            continue;
        }

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                const QString VALUE = stringEditWidget->text();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                const int VALUE = lineEditWidget->text().toInt();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                const bool VALUE = checkboxWidget->checkState();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                const QString VALUE = colorButtonWidget->
                    getButtonColor().name();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (sliderEditWidget) {
                const int VALUE = sliderEditWidget->value();
                mSettingsHelper->setIntSetting(THIS_KEY, VALUE);
            }
            continue;
        }

        // Get QComboBox for Language.
        if (THIS_VALUETYPE == COMBOBOX_VALUETYPE &&
            THIS_KEY == SettingsHelper::APP_LANGUAGE) {
            QComboBox* langComboWidget = nullptr;
            langComboWidget = qobject_cast<QComboBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (langComboWidget) {
                const QString VALUE = langComboWidget->currentText();
                mSettingsHelper->setStringSetting(THIS_KEY, VALUE);
            }
            continue;
        }
    }

    // Signal X11 thread we're updated with a settings
    // change that needs a canvas redraw. We use shorthand
    // "Any changed setting in PROPERTIES list starting from
    // background color forces canvas redraw".
    bool canvasNeedsRedraw = false;
    const int SETTINGS_SIZE = SettingsHelper::PROPERTIES.size();
    for (int index = 0; index < SETTINGS_SIZE; index++) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[index];
        if (THIS_SETTING.name == SettingsHelper::BACKGROUND_COLOR) {
            for (; index < SETTINGS_SIZE; index++) {
                if (mSettingChanges[index] == true) {
                    canvasNeedsRedraw = true;
                    break;
                }
            }
        }
    }
    sendConfigDialogUpdatedEvent(canvasNeedsRedraw);

    // Translate controls to new lang for next time.
    translateConfigDialog();

    // Done.
    mSettingChanges.fill(false);
    mApplyButton->setEnabled(false);
}

/**
 * Send an event to the X11 thread telling it to redraw
 * canvas with new user config settings.
 */
void
ConfigDialog::sendConfigDialogUpdatedEvent(
    const bool canvasNeedsRedraw) {

    XEvent event{};
    event.xclient.type = ClientMessage;
    event.xclient.message_type = mConfigUpdated;

    event.xclient.window = getWindow();
    event.xclient.format = 32;

    event.xclient.data.l[0] = canvasNeedsRedraw ? 1L : 0L;

    XSendEvent(mDisplay, getWindow(), False, NoEventMask, &event);
    XFlush(mDisplay);
}

/**
 * Show this apps "About" dialog.
 */
void
ConfigDialog::showAboutDialog() {

    mAboutDialog = new AboutDialog(this);
    mAboutDialog->show();
}
