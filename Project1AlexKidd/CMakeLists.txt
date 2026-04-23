cmake_minimum_required(VERSION 3.31.1)
project(Project1AlexKidd VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# Find the raylib package (vcpkg handles this)
find_package(raylib CONFIG REQUIRED)

# List all source files
set(SOURCES
    main.cpp
    GameManager.cpp
    PlayerManager.cpp
    MapManager.cpp
    SoundManager.cpp
    MusicManager.cpp
    Enemy.cpp
)

# Create the executable target
add_executable(Project1AlexKidd ${SOURCES})

# Copy the asset folders to the output directory after building
add_custom_command(TARGET Project1AlexKidd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Sprites"
        "$<TARGET_FILE_DIR:Project1AlexKidd>/Sprites"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Sound"
        "$<TARGET_FILE_DIR:Project1AlexKidd>/Sound"
)

# Link raylib
target_link_libraries(Project1AlexKidd PRIVATE raylib)