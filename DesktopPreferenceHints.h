
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct DesktopPreferenceHints : public QObject {
    QSlider* s;

    DesktopPreferenceHints(QSlider* slider) :
        QObject(slider), s(slider) {}

    bool eventFilter(QObject* o, QEvent* e) override {
        // Show the immediate millisecond the mouse
        // crosses into the slider.
        if (e->type() == QEvent::Enter) {
            if (!s->isSliderDown()) {
                const int VALUE = s->value();
                const QString TOOLTIP_TEXT =
                    (VALUE == -1) ? "All" : "Desktop " +
                        QString::number(VALUE + 1);
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
            return false;
        }
        // Show also on interaction.
        if (e->type() == QEvent::ToolTip ||
            e->type() == QEvent::MouseMove) {
            if (!s->isSliderDown()) {
                const int VALUE = s->value();
                const QString TOOLTIP_TEXT =
                    (VALUE == -1) ? "All" : "Desktop " +
                        QString::number(VALUE + 1);
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
        }
        return false;
    }
};

#pragma GCC diagnostic pop
