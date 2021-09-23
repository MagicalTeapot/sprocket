#include "Inspector.h"

#include <Anvil/Anvil.h>

#include <Sprocket/Scene/ecs.h>
#include <Sprocket/Scene/Loader.h>
#include <Sprocket/Scene/meta.h>
#include <Sprocket/UI/DevUI.h>
#include <Sprocket/UI/ImGuiXtra.h>
#include <Sprocket/Utility/Maths.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <type_traits>

namespace {

template <typename T>
void display_attr(
    std::string_view display_name,
    const std::unordered_map<std::string, std::string>& metadata,
    T* value,
    Anvil* editor)
{
    std::string name{display_name};
    if constexpr (std::is_same_v<T, std::string>) {
        if (metadata.contains("file_filter")) {
            spkt::ImGuiXtra::File(name, editor->window(), value, metadata.at("file_filter"));
        } else {
            spkt::ImGuiXtra::TextModifiable(*value);
        }
    } else if constexpr (std::is_same_v<T, float>) {
        if (metadata.contains("lower_limit") && metadata.contains("upper_limit")) {
            float lower = std::stof(metadata.at("lower_limit"));
            float upper = std::stof(metadata.at("upper_limit"));
            ImGui::SliderFloat(name.c_str(), value, lower, upper);    
        } else {
            ImGui::DragFloat(name.c_str(), value, 0.01f);
        }
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        ImGui::DragFloat2(name.c_str(), &value->x, 0.01f);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        if (metadata.contains("is_colour")) {
            ImGui::ColorEdit3(name.c_str(), &value->x);
        } else {
            ImGui::DragFloat3(name.c_str(), &value->x, 0.1f);
        }
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
        if (metadata.contains("is_colour")) {
            ImGui::ColorEdit4(name.c_str(), &value->x);
        } else {
            ImGui::DragFloat4(name.c_str(), &value->x, 0.1f);
        }
    } else if constexpr (std::is_same_v<T, glm::quat>) {
        spkt::ImGuiXtra::Euler(name, value);
    } else if constexpr (std::is_same_v<T, bool>) {
        ImGui::Checkbox(name.c_str(), value);
    }
}

}

void Inspector::Show(Anvil& editor)
{
    spkt::registry& registry = editor.active_scene()->registry;
    spkt::entity entity = editor.selected();

    if (!registry.valid(entity)) {
        if (ImGui::Button("New Entity")) {
            entity = registry.create();
            editor.set_selected(entity);
        }
        return;
    }
    int count = 0;

    ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "ID: %llu", entity);
    spkt::for_each_component([&]<typename T>(spkt::reflcomp<T>&& refl) {
        if (registry.has<T>(entity)) {
            auto& c = registry.get<T>(entity);
            if (ImGui::CollapsingHeader(refl.name)) {
                ImGui::PushID(count++);
                refl.for_each_attribute(c, [&](auto&& attr) {
                    display_attr(attr.display_name, attr.metadata, attr.value, &editor);
                });
                if constexpr (std::is_same_v<T, spkt::Transform3DComponent>) {
                    spkt::ImGuiXtra::GuizmoSettings(d_operation, d_mode, d_useSnap, d_snap);
                }
                if (ImGui::Button("Delete")) { registry.remove<T>(entity); }
                ImGui::PopID();
            }
        }
    });

    ImGui::Separator();

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("missing_components_list");
    }

    if (ImGui::BeginPopup("missing_components_list")) {
        spkt::for_each_component([&]<typename T>(spkt::reflcomp<T>&& refl) {
            if (!registry.has<T>(entity) && ImGui::Selectable(refl.name)) {
                registry.add<T>(entity, {});
            }
        });
        ImGui::EndMenu();
    }
    ImGui::Separator();

    ImGui::Separator();
    if (ImGui::Button("Duplicate")) {
        spkt::entity copy = spkt::copy_entity(editor.active_scene()->registry, entity);
        editor.set_selected(copy);
    }
    if (ImGui::Button("Delete Entity")) {
        registry.destroy(entity);
        editor.clear_selected();
    }
}
