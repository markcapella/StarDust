
#include "Global.h"

#include <QStyleFactory>

/**
 * Simple class to represent a ConfigDialog.
 */
ConfigDialog::ConfigDialog(QWidget* parent) : QDialog(parent) {
    resize(CONFIG_DIALOG_WIDTH, CONFIG_DIALOG_HEIGHT);
    setFixedSize(size());

    // Set the window title.
    const QString FIRST_RECENTS_NAME = mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME = mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + APP_RECENT_NAME;
    }
    TITLE += " Settings";
    setWindowTitle(QString(TITLE));

    // Set the window attributes.
    setWindowFlags(Qt::Dialog | Qt::Tool);
    setMinimumWidth(CONFIG_DIALOG_WIDTH);
    setMaximumHeight(CONFIG_DIALOG_HEIGHT);

    createConfigDialogControls();

    // Callbacks.
    connect(mConfigButtonBox, &QDialogButtonBox::accepted, this,
        &ConfigDialog::acceptConfigDialogControls);
    connect(mConfigButtonBox, &QDialogButtonBox::rejected, this,
        &ConfigDialog::reject);
    connect(mAboutButton, &QPushButton::clicked, this,
        &ConfigDialog::about);

    // X11 message Atoms.
    mConfigDialogUpdated = XInternAtom(mDisplay,
        CONFIG_DIALOG_UPDATED_EVENT.c_str(), False);
}

/**
 * Load UI form with values from .Ini.
 */
void
ConfigDialog::loadConfigDialogControls() {
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
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
    }
}

/**
 * Update any runtime dialog controls, range settings, etc.
 */
void
ConfigDialog::updateConfigDialogControls() {
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const SettingsHelper::SettingsProperty THIS_SETTING =
            SettingsHelper::PROPERTIES[i];
        const QString THIS_KEY = THIS_SETTING.name;
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;

        // Ignore Divider lines.
        if (THIS_VALUETYPE == DIVIDER_VALUETYPE) {
            continue;
        }

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
                }
            }
        }
    }
}

/**
 * Build the UI form layout.
 */
void
ConfigDialog::createConfigDialogControls() {
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
        const SettingsPropertyType THIS_VALUETYPE =
            THIS_SETTING.valueType;

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
            mFormLayout->addRow(THIS_KEY, stringEditWidget);
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = new QLineEdit(this);
            lineEditWidget->setObjectName(THIS_KEY);
            lineEditWidget->setFixedWidth(120);
            mFormLayout->addRow(THIS_KEY, lineEditWidget);
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = new QCheckBox(this);
            checkboxWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, checkboxWidget);
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = new ColorButton(
                THIS_KEY, this);
            colorButtonWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, colorButtonWidget);
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = new QSlider(Qt::Horizontal, this);
            sliderEditWidget->setObjectName(THIS_KEY);
            sliderEditWidget->setFixedWidth(120);
            mFormLayout->addRow(THIS_KEY, sliderEditWidget);

            // Nice tooltip on slow hover.
            if (THIS_KEY == SettingsHelper::AUTOHIDE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = QString::number(value) +
                        " seconds";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
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
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    // Use the calculated center of the slider for
                    // ToolTip, as updates happen during window drag and
                    // pointer is nowhere near the slider at that point.
                    const QPoint LOCAL_POSITION = sliderEditWidget->
                        rect().center(); 
                    const QPoint GLOBAL_POSITION = sliderEditWidget->
                        mapToGlobal(LOCAL_POSITION);
                    const QString TOOLTIP_TEXT = (value == -1) ?
                        "All" : "Desktop " + QString::number(value + 1);
                    QToolTip::showText(GLOBAL_POSITION, TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new DesktopPreferenceHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::BACKGROUND_OPACITY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const int VALUE_PCT = 100 * value / 255;
                    const QString TOOLTIP_TEXT =
                        QString::number(VALUE_PCT) + "%";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new OpacityHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::STAR_SATURATION) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value) + "%";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new SaturationHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::MAX_STAR_SIZE) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = QString::number(value)
                        + " pixels.";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new MaxStarSizeHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::COLOR_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new ColorChangeRateHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::SIZE_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new SizeChangeRateHints(sliderEditWidget));
                continue;
            }

            if (THIS_KEY == SettingsHelper::POSITION_CHANGE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT =
                        QString::number(value);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
                sliderEditWidget->installEventFilter(
                    new PositionChangeRateHints(sliderEditWidget));
                continue;
            }
        }
    }

    // Add the formlayout to a formcontainer.
    QWidget* formContainer = new QWidget();
    mFormLayout->setFormAlignment(Qt::AlignCenter);
    formContainer->setLayout(mFormLayout);

    // Add the formcontainer to a scrollarea.
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(formContainer);
    scrollArea->setWidgetResizable(true);

    // The whole thing wraps up into vbox layout.
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->addWidget(scrollArea);

    // Create Ok / Cancel ButtonBoxBox with an About button.
    mConfigButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
        QDialogButtonBox::Cancel, this);

    // Create AboutDialog for About button dialog.
    mAboutDialog = new AboutDialog(this);
    mAboutButton = new QPushButton(ABOUT_STRING);
    mConfigButtonBox->addButton(mAboutButton,
        QDialogButtonBox::ActionRole);

    // Set mMainLayout as "the Layout" & done.
    mMainLayout->addWidget(mConfigButtonBox);
    setLayout(mMainLayout);
}

/**
 * Callback to Save UI form values to .Ini.
 */
void
ConfigDialog::acceptConfigDialogControls() {
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
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
    }

    // Signal X11 thread we're updated.
    sendConfigDialogUpdatedEvent();

    // And done.
    accept();
}

/**
 * Send an event to the X11 thread telling it to update
 * with new user config settings.
 */
void
ConfigDialog::sendConfigDialogUpdatedEvent() {
    XEvent event{};
    event.xclient.type = ClientMessage;
    event.xclient.window = getWindow();
    event.xclient.message_type = mConfigDialogUpdated;

    event.xclient.format = 32;
    event.xclient.data.l[0] = 12345;

    XSendEvent(mDisplay, getWindow(), False, NoEventMask, &event);
    XFlush(mDisplay);
}

/**
 * Show this apps "About" dialog.
 */
void
ConfigDialog::about() {
    mAboutDialog->show();
}
