#pragma once
#include <Sprocket/Graphics/AssetManager.h>
#include <Sprocket/Graphics/Buffer.h>
#include <Sprocket/Graphics/DepthBuffer.h>
#include <Sprocket/Graphics/Shader.h>
#include <Sprocket/Graphics/VertexArray.h>
#include <Sprocket/Scene/ecs.h>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace spkt {

class Texture;

class ShadowMap
{
    AssetManager* d_assetManager;

    shader d_shader;

    glm::mat4 d_lightViewMatrix;
    glm::mat4 d_lightProjMatrix;

    DepthBuffer d_shadowMap;

    std::unique_ptr<VertexArray> d_vao;
    std::unique_ptr<Buffer> d_instanceBuffer;

    std::vector<InstanceData> d_instanceData;

public:
    ShadowMap(AssetManager* assetManager);

    void Draw(spkt::registry& registry, const glm::vec3& sunDirection, const glm::vec3& centre);

    glm::mat4 GetLightProjViewMatrix() const;
    Texture*  GetShadowMap() const;
};

}