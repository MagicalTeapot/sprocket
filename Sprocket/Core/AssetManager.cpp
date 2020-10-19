#include "AssetManager.h"

#include <filesystem>

namespace Sprocket {

AssetManager::AssetManager()
    : d_defaultMesh(std::make_shared<Mesh>())
    , d_defaultTexture(std::make_shared<Texture>())
    , d_defaultMaterial(std::make_shared<Material>())
{
}

std::shared_ptr<Mesh> AssetManager::GetMesh(const std::string& file)
{
    if (file == "") { return d_defaultMesh; }
    std::string filepath = std::filesystem::absolute(file).string();

    if (auto it = d_meshes.find(filepath); it != d_meshes.end()) {
        return it->second;
    }

    auto model = Mesh::FromFile(filepath);
    d_meshes.emplace(filepath, model);
    return model;
}

std::shared_ptr<Texture> AssetManager::GetTexture(const std::string& file)
{
    if (file == "") { return d_defaultTexture; }
    std::string filepath = std::filesystem::absolute(file).string();

    if (auto it = d_textures.find(filepath); it != d_textures.end()) {
        return it->second;
    }

    if (auto it = d_loadingTextures.find(filepath); it != d_loadingTextures.end()) {
        if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            auto texture = std::make_shared<Texture>(*(it->second.get()));
            d_loadingTextures.erase(it);
            d_textures[filepath] = texture;
            return texture;
        }
    } else {
        d_loadingTextures[filepath] = std::async(std::launch::async, [filepath]() {
            return std::make_unique<TextureData>(filepath);
        });
    }

    return d_defaultTexture;
}

std::shared_ptr<Material> AssetManager::GetMaterial(const std::string& file)
{
    if (file == "") { return d_defaultMaterial; }
    std::string filepath = std::filesystem::absolute(file).string();

    if (auto it = d_materials.find(filepath); it != d_materials.end()) {
        return it->second;
    }

    auto material = Material::FromFile(filepath);
    d_materials.emplace(filepath, material);
    return material;
}

}