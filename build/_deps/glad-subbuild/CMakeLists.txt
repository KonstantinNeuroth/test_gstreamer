# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.22.0-rc3)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(glad-populate NONE)



include(ExternalProject)
ExternalProject_Add(glad-populate
                     "UPDATE_DISCONNECTED" "False" "URL" "C:/Users/Konstantin/Documents/Code/external_projects/glad.zip"
                    SOURCE_DIR          "C:/Users/Konstantin/Documents/Code/sdl_opengl/build/_deps/glad-src"
                    BINARY_DIR          "C:/Users/Konstantin/Documents/Code/sdl_opengl/build/_deps/glad-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


