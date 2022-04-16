#pragma once

#include <glad/glad.h>

#include <vector>

namespace buffer {
/**
 * @brief the class is intended to handle the upload and drawing of a Vector of
 * Vector's
 *
 * @note during the call of upload and draw the Vector content is assumed to not
 * change
 * @note the maximum size of the data does not get handled by this class yet
 */
class Vector {
 public:
  Vector(const std::vector<float>* const vec) : gpuSize_(0), vec_(vec) {}

  /**
   * @brief uploads the reference vector to the gpu
   */
  void upload_to_active_vbo() {
    if (vec_->size() != gpuSize_) {
      resize();
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vec_->size(),
                    vec_->data());
  }

  /**
   * @brief draws the uploaded content
   */
  void draw(const int32_t numberOfValuesInVertices) const {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vec_->size() / numberOfValuesInVertices);
  }

 private:
  void resize() {
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec_->size(), nullptr,
                 GL_DYNAMIC_DRAW);
    gpuSize_ = vec_->size();
  }

  const std::vector<float>* const vec_;
  size_t gpuSize_;
};
}  // namespace buffer
