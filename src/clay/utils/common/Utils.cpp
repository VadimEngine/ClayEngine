// standard lib
#include <vector>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
// project
#include "clay/utils/common/Logger.h"
// class
#include "clay/utils/common/Utils.h"

namespace clay::utils {
    size_t Vec2Hash::operator()(const glm::ivec2& v) const {
        return std::hash<int>()(v.x) ^ std::hash<int>()(v.y);
    }



} // namespace clay::utils
