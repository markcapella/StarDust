##
# Prototype desktop widget similar to kde plasmoid. Remembers
# it's position and size on screen. Hovered Pin Button
# allows click to show or hide the widgets movement & sizing
# frame.
#
# Widget views are transparent (see-through to desktop) &
# transparent to clicks. Desktop icons for example can be
# clicked "through" the Widget draw canvas.
#
# Widget draw canvas can be customized to new views easily.
#
set(APP_NAME "StarDust")
set(APP_DESC "A colorful deep star field that twinkles and")
set(APP_DESC "${APP_DESC} slowly changes on your desktop.")

set(APP_VERSION "26.06.29")
set(APP_VERSION_MINOR "00")

set(APP_AUTHOR "Mark James Capella")
set(ORG_NAME "skyrmion.inc")
set(SOURCE_REPO "https://github.com/markcapella/${APP_NAME}")

# Enable cmake.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define escape codes for error messages.
string(ASCII 27 ESC)
set(COLOR_RED    "${ESC}[31m")
set(COLOR_GREEN  "${ESC}[32m")
set(COLOR_YELLOW "${ESC}[33m")
set(COLOR_NORMAL "${ESC}[m")

cmake_minimum_required(VERSION 3.10)
project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Enable full threading.
if (NOT DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
    message(STATUS "${COLOR_GREEN}All cores enabled.${COLOR_NORMAL}")
    cmake_host_system_information(RESULT num_cores
        QUERY NUMBER_OF_LOGICAL_CORES)
    set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${num_cores}")
endif()

# Enable ccache.
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
        CACHE STRING "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
        CACHE STRING "C++ compiler launcher")
    message(STATUS "${COLOR_GREEN}ccache enabled.${COLOR_NORMAL}")
else()
    message(STATUS "${COLOR_YELLOW}ccache not enabled.${COLOR_NORMAL}")
endif()

# Create "Required Packages" list.
set(PACKAGE_REQS "git cmake build-essential")
set(PACKAGE_REQS "${PACKAGE_REQS} pkg-config wmctrl qt6-base-dev")
set(PACKAGE_REQS "${PACKAGE_REQS} libx11-dev libxft-dev libxfixes-dev")
set(PACKAGE_REQS "${PACKAGE_REQS} libxrender-dev libxext-dev")
set(PACKAGE_REQS "${PACKAGE_REQS} libxcomposite-dev libpng-dev")

# Create Pre-reqs install message 1.
set(COULDNT_LOCATE_MSG "${COLOR_RED}")
set(COULDNT_LOCATE_MSG "${COULDNT_LOCATE_MSG}Could not locate package.")
set(COULDNT_LOCATE_MSG "${COULDNT_LOCATE_MSG}${COLOR_NORMAL}")

# Create Pre-reqs install message 2.
set(INSTALL_MSG "${COLOR_YELLOW}")
set(INSTALL_MSG "${INSTALL_MSG}Please try |sudo apt install")
set(INSTALL_MSG "${INSTALL_MSG} ${PACKAGE_REQS}|${COLOR_NORMAL}")

# Chek all.
find_package(Git QUIET)
find_program(MAKE_EXE make)

find_package(PkgConfig QUIET)
find_program(WMCTRL_EXE wmctrl)

find_package(Qt6 QUIET COMPONENTS Core Widgets)

find_package(X11 QUIET)
find_package(X11 QUIET COMPONENTS Xft)

pkg_check_modules(XFIXES QUIET xfixes)
pkg_check_modules(XRENDER QUIET xrender)
pkg_check_modules(XEXT QUIET xext)
pkg_check_modules(XCOMP QUIET xcomposite)

find_package(PNG QUIET)

if (NOT Git_FOUND OR NOT MAKE_EXE OR
    NOT ${PkgConfig_FOUND} OR NOT WMCTRL_EXE OR
    NOT ${Qt6_FOUND} OR
    NOT ${X11_FOUND} OR NOT ${X11_Xft_FOUND} OR
    NOT XFIXES_FOUND OR NOT XRENDER_FOUND OR
    NOT XEXT_FOUND OR NOT XCOMP_FOUND OR
    NOT PNG_FOUND)
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

##
# Build widget project.
#
set(WIDGET_PROJECT ${APP_NAME})

# All input sources.
set(WIDGET_SOURCES
    Star.cpp Canvas.cpp
    PinButton.cpp QuitButton.cpp MoveButton.cpp SizeButton.cpp
    ConfigDialog.cpp ColorButton.cpp AboutDialog.cpp ConfigButton.cpp
    SettingsHelper.cpp RecentsHelper.cpp DisplayHelper.cpp XHelper.cpp
    StickyWindow.cpp StickyWidgetIII.cpp
)

# Final executable.
add_executable(${WIDGET_PROJECT} ${WIDGET_SOURCES})

# Define app pngs.
set(ICON_PATH "share/icons/hicolor/scalable/apps")

# Create C++ Global #defines.
target_compile_definitions(${WIDGET_PROJECT} PRIVATE
    APP_NAME="${APP_NAME}"
    APP_DESC="${APP_DESC}"
    APP_VERSION="${APP_VERSION}"
    APP_VERSION_MINOR="${APP_VERSION_MINOR}"
    APP_AUTHOR="${APP_AUTHOR}"
    ORG_NAME="${ORG_NAME}"
    SOURCE_REPO="${SOURCE_REPO}"
    ICON_PATH="${CMAKE_INSTALL_PREFIX}/${ICON_PATH}/"
)

# All Include sources.
target_include_directories(${WIDGET_PROJECT} PRIVATE
    ${X11_INCLUDE_DIR}
    ${XFIXES_INCLUDE_DIRS}
    ${XRENDER_INCLUDE_DIRS}
    ${XEXT_INCLUDE_DIRS}
    ${XCOMP_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

# All linker libs.
target_link_libraries(${WIDGET_PROJECT} PRIVATE
    Qt6::Core Qt6::Widgets
    ${X11_LIBRARIES}
    X11::Xft
    ${XFIXES_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XEXT_LIBRARIES}
    ${XCOMP_LIBRARIES}
    ${PNG_LIBRARIES}
)

##
# Display leak report after app runs.
#
# Debug #1.
#target_compile_options(${WIDGET_PROJECT} PRIVATE
#    -g -O1 -Wall -Wextra -Wuninitialized -Wmaybe-uninitialized)
#
# Debug #2.
#target_compile_options(${WIDGET_PROJECT} PUBLIC
#    -g -O1 -fsanitize=address -fno-omit-frame-pointer)
#target_link_options(${WIDGET_PROJECT} PUBLIC
#    -fsanitize=address)

##
# Prevent sudo make install before make.
#
set(TARGET_NAME ${WIDGET_PROJECT})
install(CODE "
    set(BUILD_OUTPUT \"\${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}\")
    if (NOT EXISTS \"\${BUILD_OUTPUT}\")
        message(STATUS \"ERROR: Local build is not complete.\")
        message(FATAL_ERROR \"First make this package, then \"
            \"you can install it.\")
    endif()")

##
# Halt any old active instances.
#
install(CODE "set(APP_NAME \"${APP_NAME}\")")
install(SCRIPT haltObsolete.cmake)

##
# Install executable.
#
install(TARGETS ${WIDGET_PROJECT} DESTINATION bin)

##
# Install App & Button icons.
#
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}.png)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinIn.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}-pinIn.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinOut.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}-pinOut.png)

##
# Install .desktop.
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.desktop"
    @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.desktop"
    DESTINATION share/applications)

##
# Reset Lxqt panel, & FreeDesktop and XDG cache updates
# so the panel sees the changes immediately.
#
install(CODE "
    message(STATUS \"Updating icon and desktop caches...\")
    execute_process(COMMAND update-desktop-database)
    execute_process(COMMAND xdg-icon-resource forceupdate --theme hicolor)")

##
# Setup convenience target |make run|.
#
if (NOT TARGET run)
    add_custom_target(run
        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}"
    )
endif()

##
# Setup convenience target |make uninstall|.
#
if (NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND}
            -DAPP_NAME=${APP_NAME}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/haltObsolete.cmake
                
        COMMAND ${CMAKE_COMMAND}
            -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()
