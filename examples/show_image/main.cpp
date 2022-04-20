//#include <vector>
//
//#include "WindowManager.h"
//#include "buffer/Buffer.h"
//#include "buffer/Interface.h"
//#include "buffer/Vector.h"
//#include "shader/Program.h"
//#include "shader/def/string/pos3_col3_tex2.h"
//#include <thread>
//#include <chrono>
//#include "Texture.h"
//#include "gstreamer/videopipeline.h"
//
// int main(int argc, char* argv[]) {
//  auto win = WindowManager::instance().create("testfenster");
//  win.makeCurrent();
//
//  shader::Programm prg(
//      {shader::def::pos3_col3_tex2::vertex, shader::Shader::Type::VERTEX},
//      {shader::def::pos3_col3_tex2::frag, shader::Shader::Type::FRAGMENT});
//
//  prg.use();
//  // clang-format off
//  std::vector<float> data = {
//      -0.5, -0.5, 0,     1, 1, 1,      0, 1,
//      -0.5,  0.5, 0,     1, 1, 1,      1, 0,
//       0.5,  0  , 0,     1, 1, 1,      1, 1
//  };
//  // clang-format on
//
//
//  utilities::opengl::Buffer buf;
//  buf.bind();
//  utilities::opengl::interface({3, 3, 2});
//  buffer::Vector vec(&data);
//
//
//  std::string path_to_texture =
//  "C:\\Users\\Konstantin\\Pictures\\image-3.jpg"; Texture
//  tex(path_to_texture); buf.unbind(); auto vid =
//  GstreamManager::instance().getFakeVideo(win.getContextHandle());
//
//  while (true) {
//
//    prg.use();
//    buf.bind();
//
//    tex.bind();
//    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    vec.upload_to_active_vbo();
//    vec.draw(8);
//    win.swapBuffers();
//  }
//
//  return 0;
//}
//

#include <gst/gl/gl.h>
#include <gst/gst.h>
#include <windows.h>

#include "WindowManager.h"
#include "gstreamer/gstreamer.h"
//
//
// int main(int argc, char** argv) {
//  GstBus* bus = NULL;
//  GstCaps* caps;
//  GstElement* appsink;
//  GError* err = NULL;
//  GstGLAPI gl_api;
//
//  WindowManager::instance();
//  GstreamManager::instance();
//
//  auto win = WindowManager::instance().create("SDL and gst-plugins-gl");
//  auto gstwin =
//      GstreamManager::instance().wrapGLContext(win.getContextHandle(), "");
//
//
//  gstwin.activate(true);
//  win.makeCurrent();
//
//  auto vid = GstreamManager::instance().getFakeVideo(&gstwin);
//
//  win.makeCurrent();
//  sdl_event_loop(vid.getBus(), &win, &gstwin,&vid);
//  // win.swapBuffers();
//
//  return 0;
//}
/*
 * GStreamer
 * Copyright (C) 2009 Julien Isorce <julien.isorce@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

static GstGLShader* texture_shader;
static GLuint texture_vao;
static GLuint texture_vbo;
static GLint texture_vertex_attr;
static GLint texture_texcoord_attr;
static GstGLShader* triangle_shader;
static GLuint triangle_vao;
static GLuint triangle_vbo;
static GLint triangle_vertex_attr;
static GLint triangle_color_attr;
static GLuint index_buffer;

/* OpenGL shaders */
static const gchar* triangle_vert =
    "attribute vec4 a_position;\n\
attribute vec4 a_color;\n\
uniform float yrot;\n\
varying vec4 v_color;\n\
void main()\n\
{\n\
   mat4 rotate_y = mat4 (\n\
      cos(yrot),        0.0, -sin(yrot),    0.0,\n\
            0.0,        1.0,        0.0,    0.0,\n\
      sin(yrot),        0.0,  cos(yrot),    0.0,\n\
            0.0,        0.0,       0.0,     1.0 );\n\
   mat4 translate_x = mat4 (\n\
            1.0,        0.0,        0.0,    0.0,\n\
            0.0,        1.0,        0.0,    0.0,\n\
            0.0,        0.0,        1.0,    0.0,\n\
           -0.4,        0.0,        0.0,    1.0 );\n\
   gl_Position = translate_x * rotate_y * a_position;\n\
   v_color = a_color;\n\
}";

static const gchar* triangle_frag =
    "#ifdef GL_ES\n\
precision mediump float;\n\
#endif\n\
varying vec4 v_color;\n\
void main()\n\
{\n\
  gl_FragColor = v_color;\n\
}";

static const gchar* texture_vert =
    "attribute vec4 a_position;\n\
attribute vec2 a_texcoord;\n\
uniform float xrot;\n\
varying vec2 v_texcoord;\n\
void main()\n\
{\n\
   mat4 rotate_x = mat4 (\n\
            1.0,        0.0,        0.0, 0.0,\n\
            0.0,  cos(xrot),  sin(xrot), 0.0,\n\
            0.0, -sin(xrot),  cos(xrot), 0.0,\n\
            0.0,        0.0,        0.0, 1.0 );\n\
   gl_Position = rotate_x * a_position;\n\
   v_texcoord = a_texcoord;\n\
}";

static const gchar* texture_frag =
    "#ifdef GL_ES\n\
precision mediump float;\n\
#endif\n\
varying vec2 v_texcoord;\n\
uniform sampler2D tex;\n\
void main()\n\
{\n\
  gl_FragColor = texture2D(tex, v_texcoord);\n\
}";

/* *INDENT-OFF* */
static const float texture_vertices[] = {
    /*  X      Y      Z      S      T */
    0.1f, 0.4f,  0.0f, 0.0f, 0.0f, 0.9f, 0.4f,  0.0f, 1.0f, 0.0f,
    0.9f, -0.4f, 0.0f, 1.0f, 1.0f, 0.1f, -0.4f, 0.0f, 0.0f, 1.0f,
};

static const float triangle_vertices[] = {
    /*  X      Y      Z      R      G      B      A */
    0.0f, 0.4f, 0.0f, 1.0f,  0.0f,  0.0f, 1.0f, 0.4f, -0.4f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f, -0.4f, -0.4f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f};

static const GLushort indices[] = {0, 1, 2, 0, 2, 3};
/* *INDENT-ON* */

static guint32 sdl_message_event = -1;
struct Context__ {
  SDL_Window* sdl_window;
  GstGLContext* sdl_context;
  GstGLContext* gst_context;
  GstGLDisplay* sdl_gl_display;
  SDL_GLContext sdl_gl_context;
  GAsyncQueue* queue_input_buf;
  GAsyncQueue* queue_output_buf;
  GstVideoInfo render_video_info;
};

/* rotation angle for the triangle. */
static float rtri = 0.0f;

/* rotation angle for the quadrilateral. */
static float rquad = 0.0f;

/* A general OpenGL initialization function.  Sets all of the initial
 * parameters. */
static gboolean InitGL(
    int width, int height,
    Context__* wrap)  // We call this right after our OpenGL window is created.
{
  GError* error = NULL;
  gboolean ret = TRUE;

  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f,
               0.0f);       // This Will Clear The Background Color To Black
  glClearDepth(1.0);        // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);     // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);  // Enables Depth Testing

  /* setup the index buffer shared between the texture and triangle draw code */
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  /* setup texture shader */
  texture_shader = gst_gl_shader_new_link_with_stages(
      wrap->sdl_context, &error,
      gst_glsl_stage_new_with_string(
          wrap->sdl_context, GL_VERTEX_SHADER, GST_GLSL_VERSION_NONE,
          (GstGLSLProfile)(GST_GLSL_PROFILE_ES |
                           GST_GLSL_PROFILE_COMPATIBILITY),
          texture_vert),
      gst_glsl_stage_new_with_string(
          wrap->sdl_context, GL_FRAGMENT_SHADER, GST_GLSL_VERSION_NONE,
          (GstGLSLProfile)(GST_GLSL_PROFILE_ES |
                           GST_GLSL_PROFILE_COMPATIBILITY),
          texture_frag),
      NULL);
  if (!texture_shader) {
    g_warning("Failed to compile and link shader: %s", error->message);
    g_clear_error(&error);
    ret = FALSE;
    goto out;
  }

  /* setup buffers for drawing the texture */
  glGenVertexArrays(1, &texture_vao);
  glBindVertexArray(texture_vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

  glGenBuffers(1, &texture_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices), texture_vertices,
               GL_STATIC_DRAW);

  texture_vertex_attr =
      gst_gl_shader_get_attribute_location(texture_shader, "a_position");
  glVertexAttribPointer(texture_vertex_attr, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void*)0);

  texture_texcoord_attr =
      gst_gl_shader_get_attribute_location(texture_shader, "a_texcoord");
  glVertexAttribPointer(texture_texcoord_attr, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void*)(3 * sizeof(float)));

  glEnableVertexAttribArray(texture_vertex_attr);
  glEnableVertexAttribArray(texture_texcoord_attr);

  glBindVertexArray(0);

  /* setup triangle shader */
  triangle_shader = gst_gl_shader_new_link_with_stages(
      wrap->sdl_context, &error,
      gst_glsl_stage_new_with_string(
          wrap->sdl_context, GL_VERTEX_SHADER, GST_GLSL_VERSION_NONE,
          (GstGLSLProfile)(GST_GLSL_PROFILE_ES |
                           GST_GLSL_PROFILE_COMPATIBILITY),
          triangle_vert),
      gst_glsl_stage_new_with_string(
          wrap->sdl_context, GL_FRAGMENT_SHADER, GST_GLSL_VERSION_NONE,
          (GstGLSLProfile)(GST_GLSL_PROFILE_ES |
                           GST_GLSL_PROFILE_COMPATIBILITY),
          triangle_frag),
      NULL);
  if (!triangle_shader) {
    g_warning("Failed to compile and link shader: %s", error->message);
    gst_clear_object(&texture_shader);
    g_clear_error(&error);
    ret = FALSE;
    goto out;
  }

  /* setup buffers for drawing the triangle */
  glGenVertexArrays(1, &triangle_vao);
  glBindVertexArray(triangle_vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

  glGenBuffers(1, &triangle_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
               GL_STATIC_DRAW);

  /* reuse the index buffer */

  triangle_vertex_attr =
      gst_gl_shader_get_attribute_location(triangle_shader, "a_position");
  glVertexAttribPointer(triangle_vertex_attr, 3, GL_FLOAT, GL_FALSE,
                        7 * sizeof(float), (void*)0);

  triangle_color_attr =
      gst_gl_shader_get_attribute_location(triangle_shader, "a_color");
  glVertexAttribPointer(triangle_color_attr, 4, GL_FLOAT, GL_FALSE,
                        7 * sizeof(float), (void*)(3 * sizeof(float)));

  glEnableVertexAttribArray(triangle_vertex_attr);
  glEnableVertexAttribArray(triangle_color_attr);

  glBindVertexArray(0);

out:
  return ret;
}

static void DeinitGL(Context__* wrap) {
  gst_gl_context_activate(wrap->sdl_context, TRUE);

  gst_clear_object(&texture_shader);
  gst_clear_object(&triangle_shader);

  gst_gl_context_activate(wrap->sdl_context, FALSE);

  glDeleteBuffers(1, &triangle_vbo);
  glDeleteBuffers(1, &texture_vbo);
  glDeleteBuffers(1, &index_buffer);

  glDeleteVertexArrays(1, &triangle_vao);
  glDeleteVertexArrays(1, &texture_vao);
}

/* The main drawing function. */
static void DrawGLScene(GstVideoFrame* vframe, Context__* wrap) {
  guint texture;

  texture = *(guint*)vframe->data[0];

  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT);  // Clear The Screen And The Depth Buffer

  /* draw the triangle */
  glBindVertexArray(triangle_vao);

  gst_gl_shader_use(triangle_shader);
  gst_gl_shader_set_uniform_1f(triangle_shader, "yrot", rtri);

  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

  /* draw the textured quad */
  glBindVertexArray(texture_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  gst_gl_shader_use(texture_shader);
  gst_gl_shader_set_uniform_1i(texture_shader, "tex", 0);
  gst_gl_shader_set_uniform_1f(texture_shader, "xrot", rquad);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

  /* reset GL state we have changed to the default */
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  gst_gl_context_clear_shader(wrap->sdl_context);

  rtri +=
      1.0f * G_PI / 360.0;  // Increase The Rotation Variable For The Triangle
  rquad -= 1.0f * G_PI / 360.0;  // Decrease The Rotation Variable For The Quad

  // swap buffers to display, since we're double buffered.
  SDL_GL_SwapWindow(wrap->sdl_window);
}

/* appsink new-sample callback */
static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data) {
  Context__* wrap = (Context__*)data;
  GstSample* sample = NULL;
  GstBuffer* buf;
  GstVideoFrame* vframe;
  GstGLSyncMeta* sync_meta;

  g_signal_emit_by_name(appsink, "pull-sample", &sample, NULL);
  if (!sample) return GST_FLOW_FLUSHING;

  buf = gst_buffer_ref(gst_sample_get_buffer(sample));
  gst_sample_unref(sample);

  if (!wrap->gst_context) {
    GstMemory* mem = gst_buffer_peek_memory(buf, 0);
    wrap->gst_context =
        (GstGLContext*)gst_object_ref(((GstGLBaseMemory*)mem)->context);
  }

  sync_meta = gst_buffer_get_gl_sync_meta(buf);
  if (!sync_meta) {
    buf = gst_buffer_make_writable(buf);
    sync_meta = gst_buffer_add_gl_sync_meta(wrap->gst_context, buf);
  }
  gst_gl_sync_meta_set_sync_point(sync_meta, wrap->gst_context);

  vframe = g_new0(GstVideoFrame, 1);
  if (!gst_video_frame_map(vframe, &wrap->render_video_info, buf,
                           (GstMapFlags)(GST_MAP_READ | GST_MAP_GL))) {
    g_warning("Failed to map the video buffer");
    gst_buffer_unref(buf);
    return GST_FLOW_ERROR;
  }
  // Another reference is owned by the video frame now and we can
  // get rid of our own
  gst_buffer_unref(buf);
  g_async_queue_push(wrap->queue_input_buf, vframe);

  /* pop then unref buffer we have finished to use in sdl */
  if (g_async_queue_length(wrap->queue_output_buf) > 3) {
    vframe = (GstVideoFrame*)g_async_queue_pop(wrap->queue_output_buf);
    gst_video_frame_unmap(vframe);
    g_free(vframe);
  }

  return GST_FLOW_OK;
}

static void sync_bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
  Context__* wrap = (Context__*)data;
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_NEED_CONTEXT: {
      const gchar* context_type;

      gst_message_parse_context_type(msg, &context_type);
      g_print("got need context %s\n", context_type);

      if (g_strcmp0(context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
        GstContext* display_context =
            gst_context_new(GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
        gst_context_set_gl_display(display_context, wrap->sdl_gl_display);
        gst_element_set_context(GST_ELEMENT(msg->src), display_context);
        gst_context_unref(display_context);
      } else if (g_strcmp0(context_type, "gst.gl.app_context") == 0) {
        GstContext* app_context = gst_context_new("gst.gl.app_context", TRUE);
        GstStructure* s = gst_context_writable_structure(app_context);
        gst_structure_set(s, "context", GST_TYPE_GL_CONTEXT, wrap->sdl_context,
                          NULL);
        gst_element_set_context(GST_ELEMENT(msg->src), app_context);
        gst_context_unref(app_context);
      }
      break;
    }
    default: {
      SDL_Event event = {
          0,
      };

      event.type = sdl_message_event;
      SDL_PushEvent(&event);

      break;
    }
  }
}

static void sdl_event_loop(GstBus* bus, Context__ * wrap) {
  GstVideoFrame* vframe = NULL;
  gboolean quit = FALSE;

  SDL_GL_MakeCurrent(wrap->sdl_window, wrap->sdl_gl_context);
  SDL_GL_SetSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
  }

  if (!InitGL(640, 480, wrap)) return;

  while (!quit) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = TRUE;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          quit = TRUE;
        }
      }

      if (event.type == sdl_message_event) {
        GstMessage* msg;

        while ((msg = gst_bus_pop(bus))) {
          switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
              g_print("End-of-stream\n");
              g_print(
                  "For more information, try to run: GST_DEBUG=gl*:3 "
                  "./sdlshare\n");
              quit = TRUE;
              break;

            case GST_MESSAGE_ERROR: {
              gchar* debug = NULL;
              GError* err = NULL;

              gst_message_parse_error(msg, &err, &debug);

              g_print("Error: %s\n", err->message);
              g_error_free(err);

              if (debug) {
                g_print("Debug deails: %s\n", debug);
                g_free(debug);
              }

              quit = TRUE;
              break;
            }

            default:
              break;
          }

          gst_message_unref(msg);
        }
      }
    }

    if (g_async_queue_length(wrap->queue_input_buf) > 3) {
      GstGLSyncMeta* sync_meta;

      if (vframe) {
        g_async_queue_push(wrap->queue_output_buf, vframe);
        vframe = NULL;
      }

      while (g_async_queue_length(wrap->queue_input_buf) > 3) {
        if (vframe) {
          gst_video_frame_unmap(vframe);
          g_free(vframe);
        }
        vframe = (GstVideoFrame*)g_async_queue_pop(wrap->queue_input_buf);
      }

      sync_meta = gst_buffer_get_gl_sync_meta(vframe->buffer);
      if (sync_meta) gst_gl_sync_meta_wait(sync_meta, wrap->sdl_context);
    }

    if (vframe) DrawGLScene(vframe, wrap);
  }

  SDL_GL_MakeCurrent(wrap->sdl_window, NULL);

  if (vframe) g_async_queue_push(wrap->queue_output_buf, vframe);

  DeinitGL(wrap);
}

int main(int argc, char** argv) {
  GstPipeline* pipeline = NULL;
  HGLRC gl_context = 0;
  HDC sdl_dc = 0;

  GstBus* bus = NULL;
  GstCaps* caps;
  GstElement* appsink;
  GstGLPlatform gl_platform;
  GError* err = NULL;
  GstGLAPI gl_api;

  /* Initialize SDL for video output */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return -1;
  }

  gst_init(&argc, &argv);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  sdl_message_event = SDL_RegisterEvents(1);
  g_assert(sdl_message_event != -1);

  /* Create a 640x480 OpenGL window */
  Context__ context_wrap{};
  context_wrap.sdl_window =
      SDL_CreateWindow("SDL and gst-plugins-gl", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
  if (context_wrap.sdl_window == NULL) {
    fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  /* Create GL context and a wrapped GStreamer context around it */
  context_wrap.sdl_gl_context = SDL_GL_CreateContext(context_wrap.sdl_window);

  SDL_GL_MakeCurrent(context_wrap.sdl_window, context_wrap.sdl_gl_context);

  gl_context = wglGetCurrentContext();
  sdl_dc = wglGetCurrentDC();
  gl_platform = GST_GL_PLATFORM_WGL;
  context_wrap.sdl_gl_display = gst_gl_display_new();

  gl_api = gst_gl_context_get_current_gl_api(gl_platform, NULL, NULL);

  context_wrap.sdl_context = gst_gl_context_new_wrapped(
      context_wrap.sdl_gl_display, (guintptr)gl_context, gl_platform, gl_api);

  gst_gl_context_activate(context_wrap.sdl_context, TRUE);

  if (!gst_gl_context_fill_info(context_wrap.sdl_context, &err)) {
    fprintf(stderr, "Failed to fill in wrapped GStreamer context: %s\n",
            err->message);
    g_clear_error(&err);
    SDL_Quit();
    return -1;
  }
  SDL_GL_MakeCurrent(context_wrap.sdl_window, NULL);

  pipeline = GST_PIPELINE(gst_parse_launch(
      "videotestsrc ! glupload name=upload ! gleffects effect=5 ! "
      "appsink name=sink",
      NULL));

  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_enable_sync_message_emission(bus);
  g_signal_connect(bus, "sync-message", G_CALLBACK(sync_bus_call),
                   &context_wrap);

  context_wrap.queue_input_buf = g_async_queue_new();
  context_wrap.queue_output_buf = g_async_queue_new();

  appsink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

  GstStaticCaps render_caps = GST_STATIC_CAPS(
      "video/"
      "x-raw(memory:GLMemory),format=RGBA,width=320,height=240,framerate=("
      "fraction)30/1,texture-target=2D");

  caps = gst_static_caps_get(&render_caps);

  if (!gst_video_info_from_caps(&context_wrap.render_video_info, caps))
    g_assert_not_reached();

  g_object_set(appsink, "emit-signals", TRUE, "sync", TRUE, "caps", caps, NULL);
  g_signal_connect(appsink, "new-sample", G_CALLBACK(on_new_sample),
                   &context_wrap);
  gst_object_unref(appsink);
  gst_caps_unref(caps);

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

  sdl_event_loop(bus, &context_wrap);

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
  gst_object_unref(pipeline);

  gst_object_unref(bus);

  gst_gl_context_activate(context_wrap.sdl_context, FALSE);
  gst_object_unref(context_wrap.sdl_context);
  gst_object_unref(context_wrap.sdl_gl_display);
  if (context_wrap.gst_context) gst_object_unref(context_wrap.gst_context);

  /* make sure there is no pending gst gl buffer in the communication queues
   * between sdl and gst-gl
   */
  while (g_async_queue_length(context_wrap.queue_input_buf) > 0) {
    GstVideoFrame* vframe =
        (GstVideoFrame*)g_async_queue_pop(context_wrap.queue_input_buf);
    gst_video_frame_unmap(vframe);
    g_free(vframe);
  }

  while (g_async_queue_length(context_wrap.queue_output_buf) > 0) {
    GstVideoFrame* vframe =
        (GstVideoFrame*)g_async_queue_pop(context_wrap.queue_output_buf);
    gst_video_frame_unmap(vframe);
    g_free(vframe);
  }

  SDL_GL_DeleteContext(gl_context);

  SDL_DestroyWindow(context_wrap.sdl_window);

  SDL_Quit();

  return 0;
}