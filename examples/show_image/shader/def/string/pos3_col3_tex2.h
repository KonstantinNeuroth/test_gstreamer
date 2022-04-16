#include <string>

namespace shader {
namespace def {
namespace pos3_col3_tex2 {

  const std::string vertex =
      "#version 330 core \n"
      "layout(location = 0) in vec3 aPos;\n"
      "layout(location = 1) in vec3 aColor;\n"
      "layout(location = 2) in vec2 aTexCoord;\n"
      "\n"
      "out vec3 ourColor;\n"
      "out vec2 TexCoord;\n"
      "\n"
      "void main() {\n"
      "gl_Position = vec4(aPos, 1.0);\n"
      "ourColor = aColor;\n"
      "TexCoord = aTexCoord;\n"
      "}\n";

  const std::string frag =
      "#version 330 core \n"
      "out vec4 FragColor;\n"
      "\n"
      "in vec3 ourColor;\n"
      "in vec2 TexCoord;\n"
      "\n"
      "uniform sampler2D ourTexture;\n"
      "\n"
      "void main() { FragColor = texture(ourTexture, TexCoord); }\n";
      //"void main() { FragColor = vec4(ourColor,1); }\n";

}  // namespace 3pos_3col_2tex
}  // namespace def
}  // namespace shader