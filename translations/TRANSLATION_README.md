# StarDust
    
!['StarDustIcon'](https://github.com/markcapella/StarDust/blob/main/StarDust.png)
!['StarDust'](https://github.com/markcapella/StarDust/blob/main/screenshot.png)

    
## Stardust Translations

    
### 1) Start by changing into the translations working folder.

    cd translations


### 2) Ensure all languages you wish translated are in file:

    APP_LANGUAGES.conf


### 3) Prior to build, manually extract all your code strings from a small code pool and document into file:

    APP_STRINGS.doc


### 4) Remove comments from that file and create file:

    APP_STRINGS.txt


### 5) Trigger script to read it, translating each string and writing the English input string and it's translations.

    ./getTranslatedAppStrings

    ---> APP_STRINGS_TRANSLATED.txt


### 6) Manually review that file for errors (for example):

    [ERROR] Oops! Something went wrong and I can't translate it for you :(


### 7) Hand correct those lines using (for example):

    trans -b -j :fr "Some new or existing string."


### 8) Finally, trigger script to read APP_STRINGS_TRANSLATED.txt & produce code for the build:

    ./createTranslationHelperStrings

    ---> TranslationHelperStrings.h


### 9) The script copies the final .h header file up into your source folder.


### 10) Rebuild your project & done !

    cd .. && make
