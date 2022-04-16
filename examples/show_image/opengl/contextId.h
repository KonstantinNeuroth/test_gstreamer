#pragma once

#if defined(Q_OS_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <Wingdi.h>
    #include <GL/gl.h>
#elif defined (Q_OS_MAC)
    #include <OpenGL/OpenGL.h>
    class NSOpenGLContext;
#else
    #include <X11/Xlib.h>
    #include <GL/gl.h>
    #include <GL/glx.h>
#endif


#if defined(Q_WS_WIN)
    typedef struct _tagGLContextID
    {
        HGLRC contextId;
        HDC dc;
    } GLContextID;
#elif  defined(Q_WS_MAC)
    typedef struct _tagGLContextID
    {
        NSOpenGLContext* contextId;
    } GLContextID;
#elif defined(Q_WS_X11)
    typedef struct _tagGLContextID
    {
        GLXContext contextId;
        Display *display;
        Window wnd;
    } GLContextID;
#endif