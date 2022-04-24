#pragma once
#include <gst/gl/gl.h>
#include <gst/gst.h>

class GlContextWrapper {
 public:
  GlContextWrapper(void* contextHandle, GstGLPlatform platform, GstGLAPI api) {
    GError* err = NULL;
    display = gst_gl_display_new();
    context = gst_gl_context_new_wrapped(display, (guintptr)contextHandle,
                                         platform, api);
    activate(true);
    if (!gst_gl_context_fill_info(context, &err)) {
      g_clear_error(&err);
      // TODO PANIC
    }
  }
  ~GlContextWrapper() {
    activate(false);
    gst_object_unref(context);
    gst_object_unref(display);
  }
  GstGLContext* getContextHandle() { return context; }
  GstGLDisplay* getDisplayHandle() { return display; }
  void setContextHandle(GstGLContext* v) { context = v; }
  void setDisplayHandle(GstGLDisplay* v) { display = v; }
  void activate(bool b) { gst_gl_context_activate(context, b); }

 private:
  GstGLContext* context;
  GstGLDisplay* display;
};