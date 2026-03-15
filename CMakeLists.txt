cmake_minimum_required(VERSION 3.21)
project(AudioPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include_directories(include ${PROJECT_BINARY_DIR}/include/)
include_directories(include)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Multimedia
)

qt_add_executable(audioplayer
    include/AudioPlayer.hpp
    include/MainWindow.hpp
    include/PlaylistDelegate.hpp
    src/main.cpp
    src/AudioPlayer.cpp
    src/MainWindow.cpp
    src/PlaylistDelegate.cpp
)

qt_add_resources(audioplayer "resources"
    PREFIX "/"
    FILES
        resources/styles.qss
)

target_link_libraries(audioplayer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

if(ANDROID)
    set_target_properties(audioplayer PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
endif()

qt_policy(SET QTP0002 NEW)