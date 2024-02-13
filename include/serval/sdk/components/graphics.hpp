#ifndef SERVAL_SDK__COMPONENTS_GRAPHICS_HPP
#define SERVAL_SDK__COMPONENTS_GRAPHICS_HPP

#include "../types.hpp"

namespace components::graphics {

    struct Layer {
        std::uint8_t layer;
    };

    // An image-based visual representation
    struct Sprite {

    };

    // A single image visual representation
    struct StaticImage {
        serval::Handle image;
    };

    // Visual that is always oriented towards the camera
    struct Billboard {

    };

    // A 3D mesh-based visual representation
    struct Model {
        serval::Handle mesh;
    };

    // A material with which to render a model
    struct Material {
        glm::vec3 color;
        serval::Handle albedo;
        serval::Handle normal;
        serval::Handle metalic;
        serval::Handle roughness;
        serval::Handle ambient_occlusion;
    };

    // An emitter of light, in all directions
    struct PointLight {
        Scalar radius;
        glm::vec3 color;
        Scalar intensity;
    };

    // An emitter of light, in one direction
    struct SpotLight {
        Scalar range;
        glm::vec3 color;
        glm::vec3 direction;
        Scalar intensity;
    };

}

#endif
