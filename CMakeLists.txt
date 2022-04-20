cmake_minimum_required(VERSION 3.12)

# HEADER
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:C:/gstreamer/1.0/msvc_x86_64/lib/pkgconfig")


project(sdl_opengl
    VERSION 0.0.1
    LANGUAGES C CXX
    DESCRIPTION "try to efficently play a video in opengl"
)

# 3rd party libs
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake")
endif()

include(conan)
conan_cmake_run(
  REQUIRES
    glad/0.1.35
    sdl/2.0.20
    pkgconf/1.7.4

  BASIC_SETUP CMAKE_TARGETS
  BUILD missing
)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLIB REQUIRED IMPORTED_TARGET glib-2.0) 
pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GST_VIDEO REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
pkg_check_modules(GST_GL REQUIRED IMPORTED_TARGET gstreamer-gl-1.0)
pkg_check_modules(GST_PLUGIN_BASE REQUIRED IMPORTED_TARGET gstreamer-plugins-base-1.0)
pkg_check_modules(GST_BASE REQUIRED IMPORTED_TARGET gstreamer-base-1.0)
pkg_check_modules(GST_INSB REQUIRED IMPORTED_TARGET gstreamer-insertbin-1.0)

find_package(OpenGL REQUIRED)



# COMPILE DEFINITIONS
if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
    if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        add_compile_options(-Werror)
    endif()
else()
    add_compile_options(/permissive-)
    # We want to pass this flag to link.exe. In CMake 3.13, we can use add_link_options
    # /IGNORE:4221 # LNK4221 warns for empty cpp files. We use such files.
endif()

# SOURCES
add_subdirectory(examples/show_image)