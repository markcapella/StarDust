
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct AutoHideDelayHints : public QObject {
    QSlider* s;

    AutoHideDelayHints(QSlider* slider) :
        QObject(slider), s(slider) {}

    bool eventFilter(QObject* obj, QEvent *event) override {
        // Show the immediate millisecond the mouse
        // crosses into the slider.
        if (event->type() == QEvent::Enter) {
            if (!s->isSliderDown()) {
                const int VALUE = s->value();
                const QString TOOLTIP_TEXT =
                    QString::number(VALUE) + " seconds";
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
            return false;
        }
        // Show also on interaction.
        if (event->type() == QEvent::ToolTip ||
            event->type() == QEvent::MouseMove) {
            if (!s->isSliderDown()) {
                const int VALUE = s->value();
                const QString TOOLTIP_TEXT =
                    QString::number(VALUE) + " seconds";
                QToolTip::showText(QCursor::pos(),
                    TOOLTIP_TEXT, s);
            }
        }
        return false;
    }

};

#pragma GCC diagnostic pop
