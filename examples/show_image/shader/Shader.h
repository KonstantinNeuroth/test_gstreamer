#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "glad/glad.h"
namespace shader {
class Shader {
 public:
  enum Type { VERTEX = GL_VERTEX_SHADER, FRAGMENT = GL_FRAGMENT_SHADER };
  Shader(std::string src, Type type) { init_from_source(src, type); }
  Shader(std::string path) {
    auto len = path.length();

    if (len < 3) {
      // TODO Errorhandling
    } else {
      std::string file_type = path.substr(len - 3, len);
      std::ifstream file(path);
      std::stringstream in_stream;
      in_stream << file.rdbuf();
      file.close();

      if (file_type == ".vs") {
        init_from_source(in_stream.str(), VERTEX);
      } else if (file_type == ".fs") {
        init_from_source(in_stream.str(), FRAGMENT);
      } else {
        // TODO Errorhandling
      }
    }
  }

  ~Shader() { glDeleteShader(adr_); }

  unsigned int adress() { return adr_; }

 private:
  unsigned int adr_;
  std::string to_string(Type t) {
    if (t == VERTEX) return "VERTEX";
    if (t == FRAGMENT) return "FRAGMENT";
    return "unknown";
  }

  void init_from_source(std::string src, Type type) {
    adr_ = glCreateShader(type);

    auto tmp = src.c_str();
    glShaderSource(adr_, 1, &tmp, NULL);
    glCompileShader(adr_);

    int success;
    char infoLog[512];
    glGetShaderiv(adr_, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(adr_, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::" << to_string(type)
                << "::COMPILATION_FAILED\n " << infoLog << std::endl;
    }
  }
};
}  // namespace shader
