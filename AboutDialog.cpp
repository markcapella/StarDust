
#include "Global.h"

/**
 * Simple class to represent an AboutDialog.
 *
 */
AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(QString("About ") + QString(APP_NAME));
    setMinimumWidth(600);

    // Create overall container.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    const int FONT_BASE_SIZE = 10;

    // App title line.
    QLabel* appTitleLine = new QLabel(QString(APP_NAME), this);
    appTitleLine->setAlignment(Qt::AlignCenter);
    appTitleLine->setWordWrap(true);
    QFont appTitleFont = appTitleLine->font();
    appTitleFont.setPointSize(FONT_BASE_SIZE + 8);
    appTitleFont.setBold(true);
    appTitleFont.setItalic(true);
    appTitleLine->setFont(appTitleFont);
    QPalette appTitlePalette = appTitleLine->palette();
    appTitlePalette.setColor(QPalette::WindowText, QColor("#1ed760"));
    appTitleLine->setPalette(appTitlePalette);
    mainLayout->addWidget(appTitleLine);

    // App versions line.
    QLabel* appVersionsLine = new QLabel(QString(APP_VERSION) +
        QString(" : ") + QString(APP_VERSION_MINOR), this);
    appVersionsLine->setMargin(0);
    appVersionsLine->setAlignment(Qt::AlignCenter);
    appVersionsLine->setWordWrap(true);
    QFont appVersionsFont = appVersionsLine->font();
    appVersionsFont.setPointSize(FONT_BASE_SIZE + 2);
    appVersionsLine->setFont(appVersionsFont);
    mainLayout->addWidget(appVersionsLine);
    mainLayout->addSpacing(10);

    // App description line.
    QLabel* appDescLine = new QLabel(QString(APP_DESC), this);
    appDescLine->setAlignment(Qt::AlignCenter);
    appDescLine->setWordWrap(true);
    QFont appDescFont = appDescLine->font();
    appDescFont.setPointSize(FONT_BASE_SIZE + 4);
    appDescFont.setItalic(true);
    appDescLine->setFont(appDescFont);
    mainLayout->addWidget(appDescLine);
    mainLayout->addSpacing(10);

    // App author & org line.
    QLabel* appAuthorLine = new QLabel(QString(APP_AUTHOR) +
        QString("\n") + QString(ORG_NAME), this);
    appAuthorLine->setAlignment(Qt::AlignCenter);
    appAuthorLine->setWordWrap(true);
    QFont appAuthorFont = appAuthorLine->font();
    appAuthorFont.setPointSize(FONT_BASE_SIZE + 2);
    appAuthorLine->setFont(appAuthorFont);
    mainLayout->addWidget(appAuthorLine);
    mainLayout->addSpacing(15);

    // Credits line.
    QLabel* iconCreditsLine = new QLabel(this);
    iconCreditsLine->setText("Icon artwork provided by "
        "<a href=\"https://perchance.org/ai-icon-generator\""
            ">the Perchance Art Generator</a>");
    iconCreditsLine->setOpenExternalLinks(true);
    iconCreditsLine->setTextInteractionFlags(
        Qt::LinksAccessibleByMouse);
    iconCreditsLine->setAlignment(Qt::AlignCenter);
    iconCreditsLine->setWordWrap(true);
    QFont iconCreditsFont = iconCreditsLine->font();
    iconCreditsFont.setPointSize(FONT_BASE_SIZE);
    iconCreditsLine->setFont(iconCreditsFont);
    QPalette iconCreditsPalette = iconCreditsLine->palette();
    iconCreditsPalette.setColor(QPalette::WindowText, QColor("black"));
    iconCreditsLine->setPalette(iconCreditsPalette);
    mainLayout->addWidget(iconCreditsLine);

    // Prepare warnings.
    const QString THIS_WM_NAME(mXHelper->
        getWindowManagerName().c_str());
    const QString COMPOSITOR_NAME(mXHelper->getCompositorName());
    bool areThereWarningsYet = false;

    // App visual transparency warning line.
    if (!mXHelper->isTransparentVisually()) {
        // Add blank line before first warning.
        if (!areThereWarningsYet) {
            mainLayout->addWidget(new QLabel(QString(""), this));
            areThereWarningsYet = true;
        }
        const QString VISUAL_WM_WARNING = "NOTE: Your Window Manager "
            "(WM) & its Compositor won't allow you to transparently see "
            "through StickyWidgetIII based widgets.";
        QLabel* appVTWarnLine = new QLabel(
            QString(VISUAL_WM_WARNING), this);
        appVTWarnLine->setAlignment(Qt::AlignCenter);
        appVTWarnLine->setWordWrap(true);
        QFont appVTWarnFont = appVTWarnLine->font();
        appVTWarnFont.setPointSize(FONT_BASE_SIZE + 2);
        appVTWarnLine->setFont(appVTWarnFont);
        QPalette appVTWarnPalette = appVTWarnLine->palette();
        appVTWarnPalette.setColor(QPalette::WindowText, QColor("#B22222"));
        appVTWarnLine->setPalette(appVTWarnPalette);
        mainLayout->addWidget(appVTWarnLine);
        mainLayout->addSpacing(8);
    }

    // App pointer transparency warning line.
    if (!mXHelper->isTransparentToPointer()) {
        // Add blank line before first warning.
        if (!areThereWarningsYet) {
            mainLayout->addSpacing(20);
            areThereWarningsYet = true;
        }
        const QString POINTER_WM_WARNING = "NOTE: Your Window Manager "
            "(WM) won't allow you to click through StickyWidgetIII "
            "based widgets.";
        QLabel* appPTWarnLine = new QLabel(
            QString(POINTER_WM_WARNING), this);
        appPTWarnLine->setAlignment(Qt::AlignCenter);
        appPTWarnLine->setWordWrap(true);
        QFont appPTWarnFont = appPTWarnLine->font();
        appPTWarnFont.setPointSize(FONT_BASE_SIZE + 2);
        appPTWarnLine->setFont(appPTWarnFont);
        QPalette appPTWarnPalette = appPTWarnLine->palette();
        appPTWarnPalette.setColor(QPalette::WindowText, QColor("#B22222"));
        appPTWarnLine->setPalette(appPTWarnPalette);
        mainLayout->addWidget(appPTWarnLine);
        mainLayout->addSpacing(8);
    }

    // App tiling WM warning line.
    if (TILING_WM_NAMES.contains(THIS_WM_NAME)) {
        // Add blank line before first warning.
        if (!areThereWarningsYet) {
            mainLayout->addSpacing(20);
            areThereWarningsYet = true;
        }
        const QString TILING_WM_WARNING = "NOTE: Your Window "
            "Manager is a Tiling window manager. This may prevent "
            "you from seeing or clicking through StickyWidgetIII "
            "based widgets.";
        QLabel* appTilingWMLine = new QLabel(
            QString(TILING_WM_WARNING), this);
        appTilingWMLine->setAlignment(Qt::AlignCenter);
        appTilingWMLine->setWordWrap(true);
        QFont appTilingWMFont = appTilingWMLine->font();
        appTilingWMFont.setPointSize(FONT_BASE_SIZE + 2);
        appTilingWMLine->setFont(appTilingWMFont);
        QPalette appTilingWMPalette = appTilingWMLine->palette();
        appTilingWMPalette.setColor(QPalette::WindowText, QColor("#B22222"));
        appTilingWMLine->setPalette(appTilingWMPalette);
        mainLayout->addWidget(appTilingWMLine);
        mainLayout->addSpacing(8);
    }

    // App No Compositor warning line.
    if (COMPOSITOR_NAME.isEmpty()) {
        // Add blank line before first warning.
        if (!areThereWarningsYet) {
            mainLayout->addSpacing(20);
            areThereWarningsYet = true;
        }
        const QString COMPOSITOR_WARNING = "NOTE: Your Compositor is "
            "unknown. This may prevent you from seeing or clicking "
            "through StickyWidgetIII based widgets.";
        QLabel* compositorLine = new QLabel(
            QString(COMPOSITOR_WARNING), this);
        compositorLine->setAlignment(Qt::AlignCenter);
        compositorLine->setWordWrap(true);
        QFont compositorFont = compositorLine->font();
        compositorFont.setPointSize(FONT_BASE_SIZE + 2);
        compositorLine->setFont(compositorFont);
        QPalette compositorPalette = compositorLine->palette();
        compositorPalette.setColor(QPalette::WindowText, QColor("#B22222"));
        compositorLine->setPalette(compositorPalette);
        mainLayout->addWidget(compositorLine);
        mainLayout->addSpacing(8);
    }

    // Add blank line before WM / Compositor summary.
    mainLayout->addSpacing(7);

    // App WM and Compositor summary line.
    const QString SUMMARY_WM_WARNING = "Your Window Manager is " +
        THIS_WM_NAME + ", Your Compositor is " +
        (COMPOSITOR_NAME == "" ? "unknown" : COMPOSITOR_NAME) + ".";
    QLabel* appSummaryLine = new QLabel(QString(
        SUMMARY_WM_WARNING), this);
    appSummaryLine->setAlignment(Qt::AlignCenter);
    appSummaryLine->setWordWrap(true);
    QFont appSummaryFont = appSummaryLine->font();
    appSummaryFont.setPointSize(FONT_BASE_SIZE + 1);
    appSummaryLine->setFont(appSummaryFont);
    mainLayout->addWidget(appSummaryLine);

    // Add blank line before Buttons Box.
    mainLayout->addSpacing(20);

    // Get Buttonbox with Ok button & button to open source Repo.
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok, this);
    QPushButton* repoButton = new QPushButton("Repo", this);
    buttonBox->addButton(repoButton, QDialogButtonBox::ActionRole);

    // Connect Ok and Repo signals.
    connect(buttonBox, &QDialogButtonBox::accepted, this,
        &QDialog::accept);
    connect(repoButton, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl(SOURCE_REPO));
        this->close();
    });

    mainLayout->addWidget(buttonBox);
}

/**
 * Close the AboutDialog when the window is closed
 * by clicking top-right 'X' button.
 *
 * Accept the close event so the system knows we handled
 * the window destruction.
 */
void
AboutDialog::closeEvent(QCloseEvent* event) {
    reject();

    event->accept();
}
