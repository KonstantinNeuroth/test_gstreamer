#pragma once

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <Wingdi.h>
    #include <GL/gl.h>


    typedef struct _tagGLContextID
    {
        HGLRC contextId;
        HDC dc;
    } GLContextID;