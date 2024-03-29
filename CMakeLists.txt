cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(PolygonPaint
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(FETCHCONTENT_QUIET OFF)

# SFML
set(BUILD_SHARED_LIBS FALSE)
set(SFML_VERSION "2.5.1")
FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG        "${SFML_VERSION}"
)
FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory("${sfml_SOURCE_DIR}" "${sfml_BINARY_DIR}")
endif()

# ImGui
set(IMGUI_VERSION "1.74")
FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG        "v${IMGUI_VERSION}"
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# ImGui-SFML
set(IMGUI_SFML_VERSION "2.1")
set(BUILD_SHARED_LIBS FALSE)
FetchContent_Declare(
        imgui_sfml
        GIT_REPOSITORY "https://github.com/eliasdaler/imgui-sfml.git"
        GIT_TAG        "v${IMGUI_SFML_VERSION}"
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML CACHE BOOL FALSE)

FetchContent_GetProperties(imgui_sfml)
if(NOT imgui_sfml_POPULATED)
    FetchContent_Populate(imgui_sfml)
    add_subdirectory("${imgui_sfml_SOURCE_DIR}" "${imgui_sfml_BINARY_DIR}")
endif()

set(SOURCE
        src/main.cpp

        src/application.cpp
        src/utils/color.cpp
        src/utils/dialog.cpp
        src/utils/svg.cpp
        src/shape/layer.cpp
        src/shape/edge-bucket.cpp
        src/shape/polygon.cpp
        src/state-manager.cpp
        src/event-manager.cpp
        )

add_executable(PolygonPaint
        ${SOURCE}

        "lib/tinyfiledialogs/tinyfiledialogs.c"
        "lib/simple-svg/simple_svg.hpp"
        "lib/tinyxml2/tinyxml2.cpp"
        )

target_include_directories(PolygonPaint
        PRIVATE
        lib/tinyfiledialogs
        lib/simple-svg
        lib/tinyxml2
        )

target_link_libraries(PolygonPaint
        PRIVATE
        sfml-graphics
        sfml-system
        sfml-window
        ImGui-SFML::ImGui-SFML
        )
