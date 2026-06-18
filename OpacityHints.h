
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct OpacityHints : public QObject {
    QSlider* s;

    OpacityHints(QSlider* slider) :
        QObject(slider), s(slider) {}

    bool eventFilter(QObject* o, QEvent* e) override {
        // Show the immediate millisecond the mouse
        // crosses into the slider.
        if (e->type() == QEvent::Enter) {
            if (!s->isSliderDown()) {
                const int VALUE_PCT = 100 * s->value() / 255;
                const QString TOOLTIP_TEXT =
                    QString::number(VALUE_PCT) + "%";
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
            return false;
        }
        // Show also on interaction.
        if (e->type() == QEvent::ToolTip ||
            e->type() == QEvent::MouseMove) {
            if (!s->isSliderDown()) {
                const int VALUE_PCT = 100 * s->value() / 255;
                const QString TOOLTIP_TEXT =
                    QString::number(VALUE_PCT) + "%";
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
        }
        return false;
    }
};

#pragma GCC diagnostic pop
