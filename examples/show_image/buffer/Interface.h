#pragma once
#include <glad/glad.h>

#include <numeric>
#include <vector>

namespace utilities {
namespace opengl {

void interface(std::vector<size_t> sizes) {
  int total_size = std::accumulate(sizes.begin(), sizes.end(), 0);
  size_t offset = 0;
  for (size_t i = 0; i < sizes.size(); ++i) {
    auto current_size = sizes.at(i);

    glVertexAttribPointer(static_cast<GLuint>(i), current_size, GL_FLOAT,
                          GL_FALSE, total_size * sizeof(float),
                          (void*)(sizeof(float) * offset));
    glEnableVertexAttribArray(static_cast<GLuint>(i));
    offset += current_size;
  }
}

}  // namespace opengl
}  // namespace utilities