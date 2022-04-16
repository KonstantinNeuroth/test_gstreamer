#include <vector>

#include "WindowManager.h"
#include "buffer/Buffer.h"
#include "buffer/Interface.h"
#include "buffer/Vector.h"
#include "shader/Program.h"
#include "shader/def/string/pos3_col3_tex2.h"
#include <thread>
#include <chrono>
#include "Texture.h"

int main(int argc, char* argv[]) {
  auto win = WindowManager::instance().create("testfenster");
  win.makeCurrent();

  shader::Programm prg(
      {shader::def::pos3_col3_tex2::vertex, shader::Shader::VERTEX},
      {shader::def::pos3_col3_tex2::frag, shader::Shader::FRAGMENT});

  prg.use();
  // clang-format off
  std::vector<float> data = {
      -0.5, -0.5, 0,     1, 1, 1,      0, 1,
      -0.5,  0.5, 0,     1, 1, 1,      1, 0, 
       0.5,  0  , 0,     1, 1, 1,      1, 1
  };
  // clang-format on


  utilities::opengl::Buffer buf;
  buf.bind();
  utilities::opengl::interface({3, 3, 2});
  buffer::Vector vec(&data);

  std::string path_to_texture = "C:\\Users\\Konstantin\\Pictures\\image-3.jpg";
  Texture tex(path_to_texture);
  buf.unbind();

  while (true) {

    prg.use();
    buf.bind();

    tex.bind();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    vec.upload_to_active_vbo();
    vec.draw(8);
    win.swapBuffers();
  }

  return 0;
}
