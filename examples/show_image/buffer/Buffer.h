#pragma once

namespace utilities {
namespace opengl {

class Buffer {
 public:
  Buffer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  }

  void bind() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
  }

  void unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

 private:
  unsigned int VBO, VAO;
};

}  // namespace opengl
}  // namespace utilities