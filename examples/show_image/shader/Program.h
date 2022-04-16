#include "Shader.h"
#include "glad/glad.h"

namespace shader {
class Programm {
 public:
  Programm(Shader vertex, Shader fragment) {
    adr_ = glCreateProgram();
    glAttachShader(adr_, vertex.adress());
    glAttachShader(adr_, fragment.adress());
    glLinkProgram(adr_);

    int success;
    char infoLog[512];
    glGetProgramiv(adr_, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(adr_, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }
  }

  ~Programm() { glDeleteProgram(adr_); }

  void use() { glUseProgram(adr_); }

 private:
  unsigned int adr_;
};
}  // namespace shader