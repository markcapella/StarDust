
#pragma once

/**
 * TranslationHelper takes an english string and returns
 * a translated string for a desired language at runtime.
 *
 * Strings are collected & translated at build time,
 * then appropriately displayed at runtime.
 */
class TranslationHelper {

    public:
        TranslationHelper();

        QString getTranslationOf(const QString english);

    private:
        /**
         * Members.
         */
};
