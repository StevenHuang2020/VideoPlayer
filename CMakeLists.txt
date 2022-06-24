#-------------------------------------------------------------------------------
#   Qt Project CMake definition file
#   Author: Steven Huang
#   Date: 21/May/22
#   Location: AKL, NZ
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#   CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8.1)
set(QT_MINIMUM_VERSION 5.9)

#-------------------------------------------------------------------------------
#   SOURCE CODE
#-------------------------------------------------------------------------------

# .h files
list(APPEND HEADER_FILES
    src/mainwindow.h
    src/about.h
    src/audio_play_thread.h
    src/video_play_thread.h
    src/audio_decode_thread.h
    src/packets_sync.h
    src/video_decode_thread.h
    src/read_thread.h
    src/subtitle_decode_thread.h
    src/play_control_window.h
    src/start_play_thread.h
    src/version.h
    src/video_label.h
)

# .cpp files
list(APPEND SOURCE_FILES
    src/main.cpp
    src/about.cpp
    src/app_settings.cpp
    src/audio_decode_thread.cpp
    src/audio_play_thread.cpp
    src/ffmpeg_init.cpp
    src/imagecv_operations.cpp
    src/log.cpp
    src/mainwindow.cpp
    src/packets_sync.cpp
    src/player_skin.cpp
    src/play_control_window.cpp
    src/qimage_convert_mat.cpp
    src/qimage_operation.cpp
    src/read_thread.cpp
    src/start_play_thread.cpp
    src/subtitle_decode_thread.cpp
    src/video_decode_thread.cpp
    src/video_play_thread.cpp
    src/video_state.cpp
    src/youtube_url_dlg.cpp
    src/video_label.cpp
)


# other files, ui, resource, etc.
list(APPEND SOURCE_FILES
    src/about.ui
    src/mainwindow.ui
    src/play_control_window.ui
    src/youtube_url_dlg.ui
    src/qmake_qmake_qm_files.qrc
    src/VideoPlayer_en_001.ts
)

# on windows, append headers to source so they show up in visual studio
if(WIN32)
    LIST(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

#-------------------------------------------------------------------------------
#   PROJECT SETTINGS
#-------------------------------------------------------------------------------

set(TARGET_NAME VideoPlayer)

set(PROJECT_NAME ${TARGET_NAME})

# Set the project name
PROJECT(${PROJECT_NAME})

#-------------------------------------------------------------------------------
#   Qt Settings
#-------------------------------------------------------------------------------

# Required Qt settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(QT_USE_QTMAIN TRUE)

# Setup the cmake prefix path so Qt can be found. Use the QTDIR variable if defined
if(NOT DEFINED ENV{QTDIR})
    MESSAGE(STATUS "QTDIR environment variable not set. Be sure to set the QTDIR
    cmake cache variable to the Qt root install location")
endif()

set(QTDIR $ENV{QTDIR} CACHE STRING "Qt install path")
list(APPEND CMAKE_PREFIX_PATH ${QTDIR})

# Find the necessary Qt5 modules
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

# Test for supported Qt version
find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
if(QT_VERSION VERSION_LESS QT_MINIMUM_VERSION)
    MESSAGE(FATAL_ERROR "Minimum supported Qt version: ${QT_MINIMUM_VERSION}. Installed version: ${QT_VERSION}")
endif()

# find thread library
find_package(Threads REQUIRED)

#-------------------------------------------------------------------------------
#   RESOURCES
#-------------------------------------------------------------------------------

list(APPEND QT_RESOURCES
    src/qmake_qmake_qm_files.qrc
)

qt5_add_resources(RESOURCES ${QT_RESOURCES})

#-------------------------------------------------------------------------------
#   COMPILE FLAGS
#-------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    set(GUI_TYPE WIN32)
    add_compile_options(/MP)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++14)
endif(MSVC)


#-------------------------------------------------------------------------------
#   PREPROCESSOR DEFINITION
#-------------------------------------------------------------------------------

add_compile_definitions(UNICODE _UNICODE)

if(WIN32)
add_compile_definitions(_WINDOWS  WIN32)
endif()

#-------------------------------------------------------------------------------
#   CONFIGURATION
#-------------------------------------------------------------------------------

set(APPLICATION_NAME ${TARGET_NAME})
set(APPLICATION_VERSION ${VERSION})
set(APPLICATION_ORGANIZATION "Steven Huang")
set(APPLICATION_ORGANIZATION_DOMAIN "https://github.com/StevenHuang2020")

#-------------------------------------------------------------------------------
#   SOURCE GROUPS
#-------------------------------------------------------------------------------

source_group("Generated Files" REGULAR_EXPRESSION ".*_automoc.cpp")
source_group("Resource Files" REGULAR_EXPRESSION ".*[.][q]?rc")
source_group("Source Files" REGULAR_EXPRESSION ".*[.]cpp")
source_group("Header Files" REGULAR_EXPRESSION ".*[.]h")
source_group("Continuous-integration" REGULAR_EXPRESSION ".*[.]yml")
source_group("Form Files" REGULAR_EXPRESSION ".*[.]ui")
source_group("Translation Files" REGULAR_EXPRESSION ".*[.]ts")

#-------------------------------------------------------------------------------
#   ADDITIONAL LIBRARIES
#-------------------------------------------------------------------------------

list(APPEND LIBRARIES
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Threads::Threads
)

list(APPEND LIBRARIES
    avcodec.lib
    avutil.lib
    avformat.lib
    swscale.lib
    swresample.lib
    avfilter.lib
)

# Remove duplicate library dependencies.
list(REMOVE_DUPLICATES LIBRARIES)

#-------------------------------------------------------------------------------
#   ADDITIONAL INCLUDES
#-------------------------------------------------------------------------------
list(APPEND INCLUDE_DIRS
    "src/ffmpeg/include"
    "src/opencv/build/include"
    "src/Visual Leak Detector/include"
    "src/opencv/build/include"
)

#-------------------------------------------------------------------------------
#   ADDITIONAL LINK DIRS
#-------------------------------------------------------------------------------
list(APPEND LINK_DIRS
    "src/opencv/build/x64/vc15/lib"
    "src/ffmpeg/lib"
)

#-------------------------------------------------------------------------------
#   GENERATE EXECUTABLE
#-------------------------------------------------------------------------------

# Set additional include directories
include_directories(${INCLUDE_DIRS})

# Generate the executable
add_executable(${TARGET_NAME} ${GUI_TYPE} ${SOURCE_FILES} ${RESOURCES})

# link libraries
target_link_libraries(${TARGET_NAME} ${LIBRARIES})

# Set link directories
target_link_directories(${TARGET_NAME} PUBLIC ${LINK_DIRS})

# Use the automatically determined RPATH values
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

INSTALL(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
INCLUDE(InstallRequiredSystemLibraries)

# Windows specific build steps
if(WIN32)
    # Run winddeployqt if it can be found
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE_DIR:${TARGET_NAME}>)
endif()

# copy run time files to target directort
if(WIN32)
    # copy ffmpeg dll libraries
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/avcodec-59.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/avdevice-59.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/avfilter-8.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/avformat-59.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/avutil-57.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/postproc-56.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/swresample-4.dll"
        "${CMAKE_SOURCE_DIR}/src/ffmpeg/bin/swscale-6.dll"
        $<TARGET_FILE_DIR:${TARGET_NAME}>)

    # copy opencv dll libraries
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/src/opencv/build/x64/vc15/bin/opencv_world455$<$<CONFIG:Debug>:d>.dll"
        $<TARGET_FILE_DIR:${TARGET_NAME}>)

    # copy tools folder
     add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/tools"
        $<TARGET_FILE_DIR:${TARGET_NAME}>/tools)

    # copy res folder
     add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/res"
        $<TARGET_FILE_DIR:${TARGET_NAME}>/res)
endif()
