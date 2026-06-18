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
    widget to reveal the Pin or Close button.
    
    
!['screenshotBefore'](https://github.com/markcapella/StarDust/blob/main/screenshotBefore.png)
!['screenshotAfter'](https://github.com/markcapella/StarDust/blob/main/screenshotAfter.png)

    
!['screenshotBefore2'](https://github.com/markcapella/StarDust/blob/main/screenshotBefore2.png)
!['screenshotAfter2'](https://github.com/markcapella/StarDust/blob/main/screenshotAfter2.png)

    
## Installation.
    KDE, Ubuntu/Gnome, or OpenBox-like Window Managers are supported.
    
    Basically, an X11 based DisplayManager, with TrueColor32 capable
    Window Manager (Kwin, Openbox, Fluxbox, Xfwm4, Mutter/Gnome).
    
    Tiling managers such as Awesome may run but without pointer event
    transparency.
    
    
### Install Pre-reqs.

For Debian systems:

    sudo apt install git cmake build-essential pkg-config qt6-base-dev libx11-dev libxft-dev libxfixes-dev libxext-dev libpng-dev

For Fedora systems:

    sudo dnf install git cmake gcc gcc-c++ make pkg-config qt6-devel libX11-devel libXft-devel libXfixes-devel libXext-devel libpng-devel

### Clone StarDust source folder.

    git clone https://github.com/markcapella/StarDust

### CD into source repo.

    cd StarDust


## Basic development.

    mkdir build
    cd build
    cmake ..

    make
    make run

    sudo make install
    sudo make uninstall
    
    make clean
    cd ..
    rm -rf build
    
    
## Usage after install.
    StarDust
    
    
## markjamescapella@proton.me Rocks !
    Yeah I do.
    
    Do not taunt the StarDust.
    
