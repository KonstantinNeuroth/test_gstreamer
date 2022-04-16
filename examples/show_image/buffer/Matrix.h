#pragma once

// clang-format off
#include <glad/glad.h>
#include <glfw/glfw3.h>
// clang-format on
#include <vector>

namespace utilities {
namespace opengl {
/**
 * @brief the class is intended to handle the upload and drawing of a Vector of Vector's
 *
 * @note during the call of upload and draw the Vector content is assumed to not change
 * @note the maximum size of the data does not get handled by this class yet
 *
 * Usage:
 * 1. have the data you want to use `vector<vector<float>> data {...};`
 * 2. create the bufferobject from the data `opengl::Vector buffer(&data);`
 * 3. (optional) update the data
 * 4. note that the vertexAttributes have to be set be the user
 * 5. upload the buffer to opengl via `buffer.upload_to_active_vbo();`
 * 6. draw the uploaded data via `buffer.draw(static_cast<int32_t>(numberOfValuesInVertices)`
 */
class Vector
{
  public:
    Vector(const std::vector<std::vector<float>> *const vec) : gpuSize_(0), vec_(vec) {}

    /**
     * @brief uploads the reference vector to the gpu
     */
    void upload_to_active_vbo() {
        if (cpuSize() < gpuSize_) {
            resize(gpuSize_);
        }

        size_t offset = 0U;
        for (size_t i = 0U; i < vec_->size(); ++i) {
            glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset),
                            static_cast<GLsizeiptr>(sizeof(GLfloat))
                                * static_cast<GLsizeiptr>(vec_->at(i).size()),
                            reinterpret_cast<const void *>(vec_->at(i).data()));
            offset += sizeof(GLfloat) * vec_->at(i).size();
        }
    }

    /**
     * @brief draws the uploaded content
     */
    void draw(const int32_t numberOfValuesInVertices) const {
        size_t offset = 0U;
        for (size_t i = 0U; i < vec_->size(); ++i) {
            glDrawArrays(GL_TRIANGLE_STRIP, static_cast<GLint>(offset),
                         static_cast<GLsizei>(static_cast<int32_t>(vec_->at(i).size())
                                              / numberOfValuesInVertices));
            offset += vec_->at(i).size() / static_cast<size_t>(numberOfValuesInVertices);
        }
    }

  private:
    void resize(size_t newSize) {
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(sizeof(GLfloat)) * static_cast<GLsizeiptr>(newSize), nullptr,
                     GL_DYNAMIC_DRAW);
        gpuSize_ = newSize;
    }

    size_t cpuSize() {
        size_t out = 0;
        for (auto it = vec_->begin(); it != vec_->end(); ++it) {
            out += it->size();
        }
        return out;
    }

    const std::vector<std::vector<float>> *const vec_;
    size_t gpuSize_;
};
} // namespace opengl
} // namespace utilities
