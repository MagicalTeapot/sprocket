#include "PostProcessor.h"

#include <Sprocket/Graphics/Mesh.h>

#include <glad/glad.h>

#include <string>

namespace spkt {
namespace {

StaticMeshData GetMeshData()
{
    StaticMeshData data;

    Vertex bottomLeft;
    bottomLeft.position = {-1.0f, -1.0f, 0.0f};
    bottomLeft.textureCoords = {0.0f, 0.0f};

    Vertex bottomRight;
    bottomRight.position = {1.0f, -1.0f, 0.0f};
    bottomRight.textureCoords = {1.0f, 0.0f};

    Vertex topRight;
    topRight.position = {1.0f, 1.0f, 0.0f};
    topRight.textureCoords = {1.0f, 1.0f};

    Vertex topLeft;
    topLeft.position = {-1.0f, 1.0f, 0.0f};
    topLeft.textureCoords = {0.0f, 1.0f};

    data.vertices = {bottomLeft, bottomRight, topRight, topLeft};
    data.indices = {0, 1, 2, 0, 2, 3};
    return data;
}

}

post_processor::post_processor(int width, int height)
    : d_width(width)
    , d_height(height)
    , d_quad(std::make_unique<Mesh>(GetMeshData()))
    , d_front_buffer(std::make_unique<FrameBuffer>(width, height))
    , d_back_buffer(std::make_unique<FrameBuffer>(width, height))
    , d_effects()
{}

void post_processor::add_effect(
    std::string_view vertex_shader,
    std::string_view fragment_shader)
{
    d_effects.push_back(std::make_unique<Shader>(
        std::string(vertex_shader),
        std::string(fragment_shader)
    ));
}

void post_processor::start_frame()
{
    d_front_buffer->Bind();
}

void post_processor::end_frame()
{
    d_quad->Bind();
    for (auto& effect : d_effects) {
        // Set up the shader. TODO: make uniform uploading more general. 
        effect->Bind();
        effect->LoadFloat("targetWidth", (float)d_width);
        effect->LoadFloat("targetHeight", (float)d_height);

        // Swap buffers, so the front is always the target we render to.
        std::swap(d_front_buffer, d_back_buffer);
        d_back_buffer->BindTexture();
        d_front_buffer->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    // Finally, render to the screen
    d_back_buffer->Unbind();
    d_front_buffer->BindTexture();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

Effect::Effect(int width, int height,
               const std::string& vertShader,
               const std::string& fragShader)
    : d_frameBuffer(width, height)
    , d_shader(vertShader, fragShader)
{
}

void Effect::BindForWrite()
{
    d_frameBuffer.Bind();
}

void Effect::UnbindForWrite()
{
    d_frameBuffer.Unbind();
}

void Effect::BindForRead() const
{
    d_shader.Bind();
    d_shader.LoadFloat("targetWidth", (float)d_frameBuffer.Width());
    d_shader.LoadFloat("targetHeight", (float)d_frameBuffer.Height());
    d_frameBuffer.BindTexture();
}

void Effect::UnbindForRead() const
{
    d_frameBuffer.UnbindTexture();
    d_shader.Unbind();
}

void Effect::Draw(Effect* next)
{
    if (next) { next->BindForWrite(); }
    BindForRead();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    UnbindForRead();
    if (next) { next->UnbindForWrite(); }
}

void Effect::SetScreenSize(int width, int height)
{
    d_frameBuffer.SetScreenSize(width, height);
}

std::unique_ptr<Effect> make_gaussian_horiz_effect(int width, int height)
{
    return std::make_unique<Effect>(
        width,
        height,
        "Resources/Shaders/GaussH.vert",
        "Resources/Shaders/Gauss.frag"
    );
}

std::unique_ptr<Effect> make_gaussian_vert_effect(int width, int height)
{
    return std::make_unique<Effect>(
        width,
        height,
        "Resources/Shaders/GaussV.vert",
        "Resources/Shaders/Gauss.frag"
    );
}

std::unique_ptr<Effect> make_negative_effect(int width, int height)
{
    return std::make_unique<Effect>(
        width,
        height,
        "Resources/Shaders/Negative.vert",
        "Resources/Shaders/Negative.frag"
    );
}

PostProcessor::PostProcessor(int width, int height)
    : d_width(width)
    , d_height(height)
    , d_quad(GetMeshData())
{
    
}

void PostProcessor::AddEffect(std::unique_ptr<Effect> effect)
{
    d_effects.push_back(std::move(effect));
}

void PostProcessor::Bind() const
{
    d_effects[0]->BindForWrite();
}

void PostProcessor::Unbind() const
{
    d_effects[0]->UnbindForWrite();
}

void PostProcessor::Draw()
{
    d_quad.Bind();
    for (int i = 0; i != d_effects.size(); ++i) {
        
        // Final effect, so draw to screen.
        if (i + 1 == d_effects.size()) {
            d_effects[i]->Draw();
        } else {
            d_effects[i]->Draw(d_effects[i+1].get());
        }
    }
}

void PostProcessor::SetScreenSize(int width, int height)
{
    d_width = width;
    d_height = height;
    for (auto& effect : d_effects) {
        effect->SetScreenSize(width, height);
    }
}
    
}