
# StarDust
    
!['StarDustIcon'](https://github.com/markcapella/StarDust/blob/main/StarDust.png)
!['StarDust'](https://github.com/markcapella/StarDust/blob/main/screenshot.png)

    
## Description
    StarDust is an X11 StickyWidgetIII based desktop view.

    A colorful deep star field that twinkles and slowly
    changes on your desktop.
    
    Desktop widgets with various views can be stuck in place to
    the desktop below other windows and display custom views with
    a transparent visual background such as Clocks, Reminders,
    Lightstrands, Meters & more!
    
    Widget remembers workstation, position, size, settings etc.
    
    Cursor hover reveals PinButton that toggles widget in
    & out of "Desktop Stuck" state.
    
    When stuck to the desktop, widget ignores clicks & passes
    all mouse actions to desktop (input transparency). (You can
    double click Desktop Icons underneath widget views.)
    
    StarDust & StickyWidgets won't present an item in your
    desktop panel, nor in your system tray. Mouse hover the
    widget to reveal the Pin or Control buttons.
    
    
!['screenshotBefore2'](https://github.com/markcapella/StarDust/blob/main/screenshot2Before.png)
!['screenshotAfter2'](https://github.com/markcapella/StarDust/blob/main/screenshot2After.png)
    

!['screenshotBefore'](https://github.com/markcapella/StarDust/blob/main/screenshotBefore.png)
!['screenshotAfter'](https://github.com/markcapella/StarDust/blob/main/screenshotAfter.png)

    
## Installation.
    KDE, Ubuntu/Gnome, or OpenBox-like Window Managers are supported.
    
    Basically, an X11 based DisplayManager, with TrueColor32 capable
    Window Manager (Kwin, Openbox, Fluxbox, Xfwm4, Mutter/Gnome).
    
    Tiling managers such as Awesome may run but without pointer event
    transparency.
    
    
### Install Pre-reqs.

For Debian systems:

    sudo apt install git cmake build-essential pkg-config qt6-base-dev \
        libx11-dev libxft-dev libxfixes-dev libxext-dev libpng-dev

For Fedora systems:

    sudo dnf install git cmake gcc gcc-c++ make pkg-config qt6-devel \
        libX11-devel libXft-devel libXfixes-devel libXext-devel \
        libpng-devel

### Clone StarDust source folder.

    git clone https://github.com/markcapella/StarDust

### CD into source repo.

    cd StarDust

## Basic development.

### Install.
    ./startProj

    # Will run startProj if reqd.
    ./buildProj

    # Will run both startProj & buildProj if reqd.
    ./installProj

### Uninstall.
    ./uninstallProj

    # Same as rm -rf build
    ./cleanProj
    
    # App config.
    rm -rf ~/.config/StarDust

## Usage after install.

### GUI Desktop.
* Click the StarDust desktop icon that's added to your systems menu under "Games".

### Command Line.

    StarDust
    
## markjamescapella@proton.me Rocks !

    Yeah I do.
    
    Do not taunt the StarDust.
    
