get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

file(GLOB_RECURSE src_files
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${ProjectId}  ${src_files})
target_link_libraries(${ProjectId}  PRIVATE 
    CONAN_PKG::glad
    CONAN_PKG::sdl
    PkgConfig::GST
    PkgConfig::GST_VIDEO
    PkgConfig::GST_GL
    PkgConfig::GST_BASE
    PkgConfig::GST_PLUGIN_BASE
    PkgConfig::GST_INSB
    OpenGL::GL
    OpenGL::GLU
)
