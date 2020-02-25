#include "Graphics/GUI/Renderer2D.h"
#include <glad/glad.h>

namespace Sprocket {

void Renderer2D::render(const Model2D& model, const Shader2D& shader)
{   
    shader.bind();
    model.bind();
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount());
    model.unbind();
    shader.unbind();
}

}