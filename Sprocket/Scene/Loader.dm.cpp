#include "Loader.h"
#include "Log.h"
#include "Components.h"
#include "Maths.h"
#include "Yaml.h"
#include "Scene.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <memory>

namespace Sprocket {
namespace Loader {

void Save(const std::string& file, ecs::Registry* reg)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Entities" << YAML::BeginSeq;
    for (const auto& slot : reg->SlotInfo()) {
        out << slot;
    }
    out << YAML::EndSeq;

    out << YAML::Key << "Components" << YAML::BeginMap;
    for (auto entity : reg->Each()) {
        if (entity.Has<TemporaryComponent>()) { return; }
        out << YAML::Key << entity.Id() << YAML::Value << YAML::BeginMap;
#ifdef DATAMATIC_BLOCK SAVABLE=true
        if (entity.Has<{{Comp.Name}}>()) {
            const auto& c = entity.Get<{{Comp.Name}}>();
            out << YAML::Key << "{{Comp.Name}}" << YAML::BeginMap;
            out << YAML::Key << "{{Attr.Name}}" << YAML::Value << c.{{Attr.Name}};
            out << YAML::EndMap;
        }
#endif
        out << YAML::EndMap;
    }
    out << YAML::EndMap;
    out << YAML::EndMap;

    std::ofstream fout(file);
    fout << out.c_str();
}

void Load(const std::string& file, ecs::Registry* reg)
{
    std::ifstream stream(file);
    std::stringstream sstream;
    sstream << stream.rdbuf();
    YAML::Node data = YAML::Load(sstream.str());

    if (!data["Entities"] || !data["Components"]) {
        return; // TODO: Error checking
    }

    std::vector<ecs::Registry::Slot> slots;
    slots.reserve(data["Entities"].size());
    for (auto slot : data["Entities"]) {
        slots.push_back(slot.as<ecs::Registry::Slot>());
    }
    reg->SetSlotInfo(slots); // Also clears the reg if it was non-empty.

    auto componentData = data["Components"];
    for (auto handle : componentData) {
        u32 id = handle.first.as<u32>();
        auto components = handle.second;
        ecs::Entity e = reg->Get(id);
#ifdef DATAMATIC_BLOCK SAVABLE=true
        if (auto spec = components["{{Comp.Name}}"]) {
            {{Comp.Name}} c;
            c.{{Attr.Name}} = spec["{{Attr.Name}}"] ? spec["{{Attr.Name}}"].as<{{Attr.Type}}>() : {{Attr.Default}};
            e.Add<{{Comp.Name}}>(c);
        }
#endif
    }
}

ecs::Entity Copy(ecs::Registry* reg, ecs::Entity entity)
{
    ecs::Entity e = reg->New();
#ifdef DATAMATIC_BLOCK
    if (entity.Has<{{Comp.Name}}>()) {
        e.Add<{{Comp.Name}}>(entity.Get<{{Comp.Name}}>());
    }
#endif
    return e;
}

void Copy(ecs::Registry* source, ecs::Registry* target)
{
    for (auto entity : source->Each()) {
        Copy(target, entity);
    }
}

}
}
