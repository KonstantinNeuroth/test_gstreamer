#pragma once
#include <gst/gl/gl.h>
#include <gst/gst.h>

#include <functional>
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

static void print_if_error(GError* error) {
  if (error != NULL) {
    std::cout << "Errormessage: " << error->message << std::endl;
    g_error_free(error);
  }
}

class Pipeline {
 public:
  Pipeline(GstPipeline* pipeline, GlContextWrapper* wrap) {
    data.pipeline_ = pipeline;
    data.wrap_ = wrap;
    GError* error = NULL;

    data.bus_ = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_enable_sync_message_emission(data.bus_);
    g_signal_connect(data.bus_, "sync-message", G_CALLBACK(sync_bus_call),
                     wrap);

    data.inputbuffer = g_async_queue_new();
    data.ouputbuffer = g_async_queue_new();

    data.appsink_ = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    GstStaticCaps render_caps = GST_STATIC_CAPS(
        "video/"
        "x-raw(memory:GLMemory),format=RGBA,width=320,height=240,framerate=("
        "fraction)30/1,texture-target=2D");
    data.caps_ = gst_static_caps_get(&render_caps);

    if (!gst_video_info_from_caps(&data.render_video_info, data.caps_))
      g_assert_not_reached();

    g_object_set(data.appsink_, "emit-signals", TRUE, "sync", TRUE, "caps",
                 data.caps_, NULL);
    g_signal_connect(data.appsink_, "new-sample", G_CALLBACK(on_new_sample),
                     &data);
    gst_object_unref(data.appsink_);
    gst_caps_unref(data.caps_);

    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
  }

  ~Pipeline() {
    gst_object_unref(data.bus_);

    gst_element_set_state(GST_ELEMENT(data.pipeline_), GST_STATE_NULL);
    gst_object_unref(data.pipeline_);

    /* make sure there is no pending gst gl buffer in the communication queues
     * between sdl and gst-gl
     */
    while (g_async_queue_length(data.inputbuffer) > 0) {
      GstVideoFrame* vframe =
          (GstVideoFrame*)g_async_queue_pop(data.inputbuffer);
      gst_video_frame_unmap(vframe);
      g_free(vframe);
    }

    while (g_async_queue_length(data.ouputbuffer) > 0) {
      GstVideoFrame* vframe =
          (GstVideoFrame*)g_async_queue_pop(data.ouputbuffer);
      gst_video_frame_unmap(vframe);
      g_free(vframe);
    }
  }
  GstBus* getBus() { return data.bus_; }
  GAsyncQueue* getInputBuffer() { return data.inputbuffer; }
  GAsyncQueue* getOutputBuffer() { return data.ouputbuffer; }

 private:
  struct Data {
    GstPipeline* pipeline_ = nullptr;
    GstBus* bus_ = nullptr;
    GstElement* appsink_ = nullptr;
    GstCaps* caps_ = nullptr;
    GstVideoInfo render_video_info;
    GlContextWrapper* wrap_;
    GAsyncQueue* inputbuffer;
    GAsyncQueue* ouputbuffer;
  };
  Data data;

  static void sync_bus_call(GstBus* bus, GstMessage* msg,
                            GlContextWrapper* wrap) {
    switch (GST_MESSAGE_TYPE(msg)) {
      case GST_MESSAGE_NEED_CONTEXT: {
        const gchar* context_type;
        gst_message_parse_context_type(msg, &context_type);
        g_print("got need context %s\n", context_type);

        if (g_strcmp0(context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
          GstContext* display_context =
              gst_context_new(GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
          gst_context_set_gl_display(display_context, wrap->getDisplayHandle());
          gst_element_set_context(GST_ELEMENT(msg->src), display_context);
          gst_context_unref(display_context);
        } else if (g_strcmp0(context_type, "gst.gl.app_context") == 0) {
          GstContext* app_context = gst_context_new("gst.gl.app_context", TRUE);
          GstStructure* s = gst_context_writable_structure(app_context);
          gst_structure_set(s, "context", GST_TYPE_GL_CONTEXT,
                            wrap->getContextHandle(), NULL);
          gst_element_set_context(GST_ELEMENT(msg->src), app_context);
          gst_context_unref(app_context);
        }
        break;
      }
      default: {
        SDL_Event event = {
            0,
        };

        event.type = -1;
        SDL_PushEvent(&event);

        break;
      }
    }
  }

  static GstFlowReturn on_new_sample(GstElement* appsink, Data* d) {
    GstSample* sample = NULL;
    g_signal_emit_by_name(appsink, "pull-sample", &sample, NULL);
    if (!sample) return GST_FLOW_FLUSHING;

    GstBuffer* buf = gst_buffer_ref(gst_sample_get_buffer(sample));
    gst_sample_unref(sample);

    if (!d->wrap_->getContextHandle()) {
      GstMemory* mem = gst_buffer_peek_memory(buf, 0);
      d->wrap_->setContextHandle(
          (GstGLContext*)gst_object_ref(((GstGLBaseMemory*)mem)->context));
    }

    GstGLSyncMeta* sync_meta = gst_buffer_get_gl_sync_meta(buf);
    if (!sync_meta) {
      buf = gst_buffer_make_writable(buf);
      sync_meta =
          gst_buffer_add_gl_sync_meta(d->wrap_->getContextHandle(), buf);
    }
    gst_gl_sync_meta_set_sync_point(sync_meta, d->wrap_->getContextHandle());

    GstVideoFrame* vframe = g_new0(GstVideoFrame, 1);
    if (!gst_video_frame_map(
            vframe, &d->render_video_info, buf,
            (GstMapFlags)((int)GST_MAP_READ | (int)GST_MAP_GL))) {
      g_warning("Failed to map the video buffer");
      gst_buffer_unref(buf);
      return GST_FLOW_ERROR;
    }
    // Another reference is owned by the video frame now and we can
    // get rid of our own
    gst_buffer_unref(buf);
    g_async_queue_push(d->inputbuffer, vframe);

    /* pop then unref buffer we have finished to use in sdl */
    if (g_async_queue_length(d->ouputbuffer) > 3) {
      vframe = (GstVideoFrame*)g_async_queue_pop(d->ouputbuffer);
      gst_video_frame_unmap(vframe);
      g_free(vframe);
    }

    return GST_FLOW_OK;
  }
};

class GstreamManager {
 public:
  static GstreamManager& instance() {
    static GstreamManager instance_;
    return instance_;
  }

  Pipeline getFakeVideo(GlContextWrapper* wrap) {
    GError* error = NULL;

    GstPipeline* pipeline =
        GST_PIPELINE(gst_parse_launch("videotestsrc ! "
                                      "glupload name=upload ! "
                                      "gleffects effect=5 ! "
                                      "appsink name=sink",
                                      &error));

    print_if_error(error);

    Pipeline out(pipeline, wrap);
    return out;
  }

  GlContextWrapper wrapGLContext(void* contextHandle, std::string platform) {
    // TODO support other platforms

    auto plf = GST_GL_PLATFORM_WGL;
    auto api = gst_gl_context_get_current_gl_api(plf, NULL, NULL);
    return GlContextWrapper(contextHandle, plf, api);
  }

  GstreamManager(GstreamManager const&) = delete;
  void operator=(GstreamManager const&) = delete;

 private:
  GstreamManager() {
    gst_init(NULL, NULL);
    guint ma, mi, mi2, mi3;
    gst_version(&ma, &mi, &mi2, &mi3);
    std::cout << "GStreamer Version: " << ma << ":" << mi << ":" << mi2 << ":"
              << mi3 << std::endl;
  }
  ~GstreamManager() { gst_deinit(); }
};