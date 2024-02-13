#ifndef SERVAL_SDK__COMPONENTS_MOTION_HPP
#define SERVAL_SDK__COMPONENTS_MOTION_HPP

#include "../types.hpp"

namespace components::motion {

    // Acceleration to be applied to a dynamic-body
    struct Steering {
        glm::vec3 linear;
        serval::Scalar angular;
    };

    // Velocity and rotation to be applied to a dynamic-body or character
    struct Kinematic {
        glm::vec3 velocity;
        serval::Scalar rotation;
    };

}

#endif