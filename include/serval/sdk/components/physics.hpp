#ifndef SERVAL_SDK__COMPONENTS_PHYSICS_HPP
#define SERVAL_SDK__COMPONENTS_PHYSICS_HPP

#include "../types.hpp"

namespace components::physics {

    // A static (immobile) physics object
    struct StaticBody {
        serval::Handle shape;
    };

    // A physics object whose motion is dictated by the physics engine
    struct DynamicBody {
        serval::Handle shape;
        Scalar mass;
    };

    // A physics object whose motion is externally controlled
    struct Character {
        serval::Handle shape;
        Scalar mass;
    };

    // A static body that only characters collide with
    struct CharacterCollider {
        serval::Handle shape;
    };

    // Entity sends signal when a physics body collides with it
    struct CollisionSensor {
        serval::Id on_collision;
        std::uint8_t collision_mask;
    };

    // Entity sends signal when a physics body begins or ends contact with it
    struct ContactSensor {
        serval::Id on_contact_begin;
        serval::Id on_contact_end;
        std::uint8_t contact_mask;
    };

    // A region which emits an event when a physics body enters or exits its area
    struct TriggerRegion {
        serval::Handle shape;
        serval::Id on_enter;
        serval::Id on_exit;
        std::uint8_t trigger_mask;
    };

}

#endif