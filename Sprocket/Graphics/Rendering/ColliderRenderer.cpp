#include "ColliderRenderer.h"
#include "Maths.h"
#include "RenderContext.h"
#include "Camera.h"
#include "Components.h"
#include "Texture.h"
#include "Types.h"

#include <glad/glad.h>

namespace spkt {

ColliderRenderer::ColliderRenderer()
    : d_vao(std::make_unique<VertexArray>())
    , d_shader("Resources/Shaders/Collider.vert", "Resources/Shaders/Collider.frag")
{
}

void ColliderRenderer::Draw(
    const glm::mat4& proj,
    const glm::mat4& view,
    Scene& scene)
{
    RenderContext rc;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    u32 MAX_NUM_LIGHTS = 5;

    d_shader.Bind();
    d_shader.LoadMat4("u_proj_matrix", proj);
    d_shader.LoadMat4("u_view_matrix", view);
    
    static auto s_cube = Mesh::FromFile("Resources/Models/Cube.obj");
    d_vao->SetModel(s_cube.get());
    for (auto entity : scene.view<BoxCollider3DComponent>()) {
        const auto& c = entity.get<BoxCollider3DComponent>();
        auto tr = entity.get<Transform3DComponent>();
        glm::mat4 transform = Maths::Transform(tr.position, tr.orientation);
        transform *= Maths::Transform(c.position, c.orientation);
        transform = glm::scale(transform, c.halfExtents);
        if (c.applyScale) {
            transform = glm::scale(transform, tr.scale);
        }
        d_shader.LoadMat4("u_model_matrix", transform);
        d_vao->Draw();
    }

    static auto s_sphere = Mesh::FromFile("Resources/Models/LowPolySphere.obj");
    d_vao->SetModel(s_sphere.get());
    for (auto entity : scene.view<SphereCollider3DComponent>()) {
        const auto& c = entity.get<SphereCollider3DComponent>();
        auto tr = entity.get<Transform3DComponent>();
        glm::mat4 transform = Maths::Transform(tr.position, tr.orientation);
        transform *= Maths::Transform(c.position, c.orientation);
        transform = glm::scale(transform, {c.radius, c.radius, c.radius});
        d_shader.LoadMat4("u_model_matrix", transform);
        d_vao->Draw();
    }

    static auto s_hemisphere = Mesh::FromFile("Resources/Models/Hemisphere.obj");
    static auto s_cylinder = Mesh::FromFile("Resources/Models/Cylinder.obj");

    for (auto entity : scene.view<CapsuleCollider3DComponent>()) {
        const auto& c = entity.get<CapsuleCollider3DComponent>();

        {  // Top Hemisphere
            auto tr = entity.get<Transform3DComponent>();
            glm::mat4 transform = Maths::Transform(tr.position, tr.orientation);
            transform *= Maths::Transform(c.position, c.orientation);
            transform = glm::translate(transform, {0.0, c.height/2, 0.0});
            transform = glm::scale(transform, {c.radius, c.radius, c.radius});
            d_shader.LoadMat4("u_model_matrix", transform);
            d_vao->SetModel(s_hemisphere.get());
            d_vao->Draw();
        }

        {  // Middle Cylinder
            auto tr = entity.get<Transform3DComponent>();
            glm::mat4 transform = Maths::Transform(tr.position, tr.orientation);
            transform *= Maths::Transform(c.position, c.orientation);
            transform = glm::scale(transform, {c.radius, c.height, c.radius});
            d_shader.LoadMat4("u_model_matrix", transform);
            d_vao->SetModel(s_cylinder.get());
            d_vao->Draw();
        }

        {  // Bottom Hemisphere
            auto tr = entity.get<Transform3DComponent>();
            glm::mat4 transform = Maths::Transform(tr.position, tr.orientation);
            transform *= Maths::Transform(c.position, c.orientation);
            transform = glm::translate(transform, {0.0, -c.height/2, 0.0});
            transform = glm::rotate(transform, glm::pi<float>(), {1, 0, 0});
            transform = glm::scale(transform, {c.radius, c.radius, c.radius});
            d_shader.LoadMat4("u_model_matrix", transform);
            d_vao->SetModel(s_hemisphere.get());
            d_vao->Draw();
        }
    }

    d_shader.Unbind();
}

void ColliderRenderer::Draw(apx::handle camera, Scene& scene)
{
    glm::mat4 proj = spkt::make_proj(camera);
    glm::mat4 view = spkt::make_view(camera);
    Draw(proj, view, scene);
}

}