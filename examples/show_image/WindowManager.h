#pragma once
#define SDL_MAIN_HANDLED
// clang-format off
// include order matters here
#include "glad/glad.h"
#include <SDL.h>
#include <SDL_opengl.h>
// clang-format on

#include <iostream>
#include <string>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class Window {
 public:
  Window(std::string name) {
    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) std::cout << "Could not create video." << std::endl;

    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    makeCurrent();
    // Initialize
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
      std::cout << "Failed to initialize OpenGL context" << std::endl;
    }
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

  }

  ~Window() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
  }

  void makeCurrent() { SDL_GL_MakeCurrent(window, context); }

  void swapBuffers() { SDL_GL_SwapWindow(window); }

  void* getContextHandle() { return context; }

 private:
  SDL_Window* window;
  SDL_GLContext context;
};

class WindowManager {
 public:
  static WindowManager& instance() {
    static WindowManager instance_;
    return instance_;
  }

  Window create(std::string name) { return Window(name); }
  Window* createNew(std::string name) { return new Window(name); }

  WindowManager(WindowManager const&) = delete;
  void operator=(WindowManager const&) = delete;

 private:
  WindowManager() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cout << "Video initialization failed: " << SDL_GetError()
                << std::endl;
    }
    SDL_GL_LoadLibrary(NULL);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  }
  ~WindowManager() { SDL_Quit(); }
};