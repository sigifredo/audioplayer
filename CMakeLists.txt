cmake_minimum_required(VERSION 3.21)
project(AudioPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Multimedia
)

qt_add_executable(AudioPlayer
    include/AudioPlayer.hpp
    include/MainWindow.hpp
    src/main.cpp
    src/AudioPlayer.cpp
    src/MainWindow.cpp
)

qt_add_resources(AudioPlayer "resources"
    PREFIX "/"
    FILES
        resources/styles.qss
)

target_link_libraries(AudioPlayer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)

if(ANDROID)
    set_target_properties(AudioPlayer PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
endif()
