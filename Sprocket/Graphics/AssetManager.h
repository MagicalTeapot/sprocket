#pragma once
#include <Sprocket/Graphics/Material.h>
#include <Sprocket/Graphics/mesh.h>
#include <Sprocket/Graphics/Texture.h>

#include <filesystem>
#include <future>
#include <memory>
#include <ranges>
#include <string_view>
#include <string>
#include <unordered_map>

namespace spkt {

template <typename T>
class basic_asset_loader
{
public:
    using asset_type = T;
    using data_type = typename T::data_type;

private:
    std::unordered_map<std::string, std::future<data_type>> d_loading;
    std::unordered_map<std::string, asset_type>             d_assets;
    asset_type                                              d_default;

public:
    basic_asset_loader() = default;

    bool is_loading() const { return !d_loading.empty(); }
    auto view() const { return std::views::all(d_assets); }

    const asset_type& get(std::string_view file)
    {
        if (file == "") { return d_default; }
        std::string filepath = std::filesystem::absolute(file).string();
        if (auto it = d_loading.find(filepath); it != d_loading.end()) {
            if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                auto rc = d_assets.emplace(filepath, it->second.get());
                d_loading.erase(it);
                return rc.first->second;
            }
        } else {
            const auto loader = [filepath]() { return data_type::load(filepath); };
            d_loading.emplace(filepath, std::async(std::launch::async, loader));
        }

        return d_default;
    }
};

class AssetManager
{
    spkt::basic_asset_loader<spkt::static_mesh>   d_static_meshes;
    spkt::basic_asset_loader<spkt::animated_mesh> d_animated_meshes;
    spkt::basic_asset_loader<spkt::texture>       d_textures;
    
    // Composites
    std::unordered_map<std::string, const std::unique_ptr<spkt::Material>> d_materials;
    std::unique_ptr<spkt::Material> d_defaultMaterial;

public:
    AssetManager();

    const static_mesh&   get_static_mesh   (std::string_view file) { return d_static_meshes.get(file); }
    const animated_mesh& get_animated_mesh (std::string_view file) { return d_animated_meshes.get(file); }
    const texture&       get_texture       (std::string_view file) { return d_textures.get(file); }
    const Material&      get_material      (std::string_view file);

    auto static_meshes()   { return d_static_meshes.view(); }
    auto animated_meshes() { return d_animated_meshes.view(); }
    auto textures()        { return d_textures.view(); }
    auto materials()       { return std::views::all(d_materials); }

    bool is_loading_static_meshes() const { return d_static_meshes.is_loading(); }
    bool is_loading_animated_meshes() const { return d_animated_meshes.is_loading(); }
    bool is_loading_textures() const { return d_textures.is_loading(); }
    bool is_loading_anything() const;
};

}