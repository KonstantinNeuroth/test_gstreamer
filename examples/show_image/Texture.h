#include <string>

#include "glad/glad.h"
#include "stb/image.h"

class Texture {
 public:
  Texture(std::string path) : error_(true) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    int width;
    int height;
    int nrChannels;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
      error_ = false;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
  }

  void bind() { glBindTexture(GL_TEXTURE_2D, id_); }
  bool error() { return error_; }

 private:
  bool error_;
  unsigned int id_;
};