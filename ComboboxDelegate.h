
#pragma once

#include <QStyledItemDelegate>
#include <QPainter>


/**
 * Styling class so QDropdown hovered item is light blue.
 *
 * The default light gray is almost invisible.
 */
class ComboboxDelegate : public QStyledItemDelegate {

    // White text over a light blue background.
    const QColor HOVERED_TEXT_COLOR = QColor("white");
    const QColor HOVERED_BACKGROUND_COLOR = QColor("#429afd");

    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        void paint(QPainter* painter, const QStyleOptionViewItem& option,
            const QModelIndex& index) const override {

            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);

            // Set our styling for hovered items.
            if (opt.state & QStyle::State_MouseOver) {
                opt.state |= QStyle::State_Selected;
                opt.palette.setColor(QPalette::HighlightedText,
                    HOVERED_TEXT_COLOR);
                opt.palette.setColor(QPalette::Highlight,
                    HOVERED_BACKGROUND_COLOR);
                QStyledItemDelegate::paint(painter, opt, index);
                return;
            }

            // Else, paint with "Not-selected style", & default
            // window text color.
            opt.state &= ~QStyle::State_Selected;
            opt.palette.setColor(QPalette::Text, opt.palette.color(
                QPalette::WindowText));

            QStyledItemDelegate::paint(painter, opt, index);
        }
};
