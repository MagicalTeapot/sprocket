#pragma once
#include <apecs.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <array>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>

struct physics_runtime;
struct rigid_body_runtime;
struct collider_runtime;

namespace spkt {
namespace lua { class script; }

using entity = apx::entity;
const auto null = apx::null;

class input_store;

struct particle
{
    glm::vec3 position      = {0.0, 0.0, 0.0};
    glm::vec3 velocity      = {0.0, 0.0, 0.0};
    glm::vec3 acceleration  = {0.0, 0.0, 0.0};
    glm::vec3 scale         = {1.0, 1.0, 1.0};
    double life = -1.0;
};

static constexpr std::size_t NUM_PARTICLES = 10000;

// Components
DATAMATIC_BEGIN
struct {{Comp::name}}
{
    {{Attr::type}} {{Attr::name}} = {{Attr::default}};
};

DATAMATIC_END
using registry = apx::registry<
DATAMATIC_BEGIN
    {{Comp::name}}{{Comp::if_not_last(",")}}
DATAMATIC_END
>;

}
