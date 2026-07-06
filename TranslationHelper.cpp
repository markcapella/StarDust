
#include "Global.h"

/**
 * TranslationHelper takes an english string and returns
 * a translated string for a desired language at runtime.
 *
 * Strings are collected & translated at build time,
 * then appropriately displayed at runtime.
 */
TranslationHelper::TranslationHelper() {
}

QString
TranslationHelper::getTranslationOf(const QString english) {

    const int TRANSLATIONS_SIZE = ALL_TRANSLATIONS.size();
    for (int i = 0; i < TRANSLATIONS_SIZE; i++) {
        const QStringList TRANSLATION_SET = ALL_TRANSLATIONS[i];

        if (TRANSLATION_SET.at(0) == english) {
            const QString LANG = mSettingsHelper->
                getStringSetting(SettingsHelper::APP_LANGUAGE);
            const int LANG_INDEX = ALL_LANGUAGES.indexOf(LANG);
            return TRANSLATION_SET.at(LANG_INDEX);
        }
    }

    // Return obvious error if not found.
    return "<translation error>";
}
