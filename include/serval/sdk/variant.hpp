#ifndef SERVAL_SDK__VARIANT_HPP
#define SERVAL_SDK__VARIANT_HPP

#include "types.hpp"
#include <variant>
#include <optional>
#include <type_traits>

/**
 * @brief Tools for managing variant types in a plugin-safe manner.
 * Utilities for converting between enum type names, integer ID's, C++ types, sizes, std::variants, etc
 * 
 */
namespace serval::variant {
    using Type = serval::DataType;
    using TypeId = serval::DataTypeID;

    struct Invalid {};
    
    using Container = std::variant<std::uint8_t, bool, std::int32_t, std::int64_t, serval::Scalar, entt::entity, serval::Id, glm::vec2, glm::vec3, glm::vec4, glm::quat, serval::ContainerHandle, serval::Handle, Invalid>;

    static constexpr TypeId to_id (Type type) {
        return static_cast<TypeId>(type);
    }

    static constexpr Type from_id (TypeId type_id) {
        return static_cast<Type>(type_id);
    }

    struct detail {
        template <Type> struct TypeOf { using Type = void; };
        template <> struct TypeOf<Type::Byte> { using Type = std::uint8_t; };
        template <> struct TypeOf<Type::Boolean> { using Type = bool; };
        template <> struct TypeOf<Type::Integer> { using Type = std::int32_t; };
        template <> struct TypeOf<Type::Integer64> { using Type = std::int64_t; };
        template <> struct TypeOf<Type::Scalar> { using Type = serval::Scalar; };
        template <> struct TypeOf<Type::Entity> { using Type = entt::entity; };
        template <> struct TypeOf<Type::Id> { using Type = serval::Id; };
        template <> struct TypeOf<Type::Vec2> { using Type = glm::vec2; };
        template <> struct TypeOf<Type::Vec3> { using Type = glm::vec3; };
        template <> struct TypeOf<Type::Vec4> { using Type = glm::vec4; };
        template <> struct TypeOf<Type::Rotation> { using Type = glm::quat; };
        template <> struct TypeOf<Type::Container> { using Type = serval::ContainerHandle; };
        template <> struct TypeOf<Type::Handle> { using Type = serval::Handle; };
    };

    /**
     * @brief Convert a Type enum into a C++ type
     * 
     * @tparam T 
     */
    template <Type T>
    using TypeOf = typename detail::TypeOf<T>::Type;

    template <Type T> static constexpr std::size_t size () {
        if constexpr (T == Type::Invalid) {
            return 0;
        } else {
            return sizeof(TypeOf<T>);
        }
    }
    static constexpr std::size_t size (Type type) {
        switch (type) {
            case Type::Byte: return size<Type::Byte>();
            case Type::Boolean: return size<Type::Boolean>();
            case Type::Integer: return size<Type::Integer>();
            case Type::Integer64: return size<Type::Integer64>();
            case Type::Scalar: return size<Type::Scalar>();
            case Type::Entity: return size<Type::Entity>();
            case Type::Id: return size<Type::Id>();
            case Type::Vec2: return size<Type::Vec2>();
            case Type::Vec3: return size<Type::Vec3>();
            case Type::Vec4: return size<Type::Vec4>();
            case Type::Rotation: return size<Type::Rotation>();
            case Type::Container: return size<Type::Container>();
            case Type::Handle: return size<Type::Handle>();
            default: return size<Type::Invalid>();
        }
    }

    /**
     * @brief Convert a C++ type to a compatible Variant::Type
     * 
     * @tparam T Native type
     * @return constexpr Type The enum type value that most closely matches the native type 
     */
    template <typename T> static constexpr Type type_of () {
        if constexpr (std::is_same_v<T, bool>) {
            return Type::Boolean;
        } else if constexpr (std::is_same_v<T, std::uint8_t>  || std::is_same_v<T, std::byte>) {
            return Type::Byte;
        } else if constexpr (std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::uint32_t> ||  std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::int32_t>) {
            return Type::Integer;
        } else if constexpr (std::is_same_v<T, std::uint64_t> || std::is_same_v<T, std::int64_t>) {
            return Type::Integer64;
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return Type::Scalar;
        } else if constexpr (std::is_same_v<T, entt::entity>) {
            return Type::Entity;
        } else if constexpr (std::is_same_v<T, serval::Id>) {
            return Type::HashId;
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return Type::Vec2;
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return Type::Vec3;
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return Type::Vec4;
        } else if constexpr (std::is_same_v<T, glm::quat>) {
            return Type::Rotation;
        } else if constexpr (std::is_same_v<T, serval::Handle>) {
            return Type::Handle;
        } else if constexpr (std::is_same_v<T, serval::ContainerHandle>) {
            return Type::Container;
        } else {
            return Type::Invalid;
        }
    }
    template <typename T> static constexpr Type type_of (T) {
        return type_of<T>();
    }

    /**
     * @brief Get a C++ type that is safely convertable to and from T
     * 
     * @tparam T 
     */
    template <typename T> using ToType = TypeOf<type_of<T>()>;

    /**
     * @brief Get the Type of the contents of a variant
     * 
     * @param value 
     * @return constexpr Type 
     */
    static constexpr Type type_of (const Container& value) {
        return std::visit(
            [](const auto& underlying) {
                return type_of<std::decay_t<decltype(underlying)>>();
            }, value
        );
    }

    /**
     * @brief Get the Type of the contents of an optional variant
     * 
     * @param value 
     * @return constexpr Type 
     */
    static constexpr Type type_of (const std::optional<Container>& optional) {
        if (optional.has_value()) {
            return std::visit(
                [](const auto& underlying) {
                    return type_of<std::decay_t<decltype(underlying)>>();
                }, optional.value()
            );
        } else {
            return Type::Invalid;
        }
    }

    /**
     * @brief Cast a void* to a C++ type
     * WARNING: ptr must point to a value of type TypeOf<type_of<T>>, no checking is performed to validate
     * 
     * @tparam T 
     * @param ptr 
     * @return T
     */
    template <typename T> static T cast (const void* ptr) {
        constexpr auto TypeValue = type_of<T>(); 
        static_assert(TypeValue != Type::Invalid, "Tried to convert void* to value with unsupported type");
        return T{*reinterpret_cast<const TypeOf<TypeValue>*>(ptr)};
    }

    /**
     * @brief Cast void* to a typed pointer
     * 
     * @tparam T 
     * @param ptr 
     * @return TypeOf<T>* 
     */
    template <Type T> static TypeOf<T>* ptr_cast (const void* ptr) {
        static_assert(T != Type::Invalid, "Tried to convert void* to invalid type");
        return reinterpret_cast<const TypeOf<T>*>(ptr);
    }

    /**TypeValue
     * @brief Cast a void* to an std::variant
     * 
     * @param type 
     * @param ptr 
     * @return Container 
     */
    static Container cast (Type type, const void* const ptr) {
        switch (type) {
            case Type::Byte: return {cast<TypeOf<Type::Byte>>(ptr)};
            case Type::Boolean: return {cast<TypeOf<Type::Boolean>>(ptr)};
            case Type::Integer: return {cast<TypeOf<Type::Integer>>(ptr)};
            case Type::Integer64: return {cast<TypeOf<Type::Integer64>>(ptr)};
            case Type::Scalar: return {cast<TypeOf<Type::Scalar>>(ptr)};
            case Type::Entity: return {cast<TypeOf<Type::Entity>>(ptr)};
            case Type::Id: return {cast<TypeOf<Type::Id>>(ptr)};
            case Type::Vec2: return {cast<TypeOf<Type::Vec2>>(ptr)};
            case Type::Vec3: return {cast<TypeOf<Type::Vec3>>(ptr)};
            case Type::Vec4: return {cast<TypeOf<Type::Vec4>>(ptr)};
            case Type::Rotation: return {cast<TypeOf<Type::Rotation>>(ptr)};
            case Type::Container: return {cast<TypeOf<Type::Container>>(ptr)};
            case Type::Handle: return {cast<TypeOf<Type::Handle>>(ptr)};
            default: return {Invalid{}};
        }
    }

    /**
     * @brief Gets a const void* to the contents of a variant
     * 
     * @param value 
     * @return const void* 
     */
    static const void* ptr (const Container& value) {
        return std::visit(
            [](const auto& underlying) -> const void* {
                using T = std::decay_t<decltype(underlying)>;
                if constexpr (std::is_same_v<T, Invalid>) {
                    return nullptr;
                } else {
                    return reinterpret_cast<const void*>(&underlying);
                }
            }, value
        );
    }

    static bool copy_into (const Container& source, Type destination_type, void* destination) {
        return std::visit(
            [destination_type, destination](const auto& underlying) {
                using T = std::decay_t<decltype(underlying)>;
                if constexpr (std::is_same_v<T, Invalid>) {
                    // Don't copy invalid
                    return false;
                } else {
                    auto source_type = serval::variant::type_of<T>();
                    if (source_type != destination_type) {
                        return false;
                    }
                    *reinterpret_cast<T*>(destination) = underlying;
                    return true;
                }
            }, source
        );
    }

    /**
     * @brief Test if a variant holds a valid value
     * 
     * @param value The variant to check
     * @return true The variant contains a valid value
     * @return false The variant contains a value of type Type::Invalid
     */
    static bool is_valid (const Container& value) {
        return !std::holds_alternative<serval::variant::Invalid>(value);
    }
    /**
     * @brief Test if a variant holds a valid value
     * 
     * @param value The variant to check
     * @return true The variant contains a value of type Type::Invalid
     * @return false The variant contains a valid value
     */
    static bool is_invalid (const Container& value) {
        return std::holds_alternative<serval::variant::Invalid>(value);
    }
}

#endif