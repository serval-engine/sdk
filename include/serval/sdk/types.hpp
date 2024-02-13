#ifndef SERVAL_SDK__TYPES_HPP
#define SERVAL_SDK__TYPES_HPP

#include "macros.hpp"

// From std::
#include <cstdint>

// From third-party dependencies
#include <entt/entity/entity.hpp>
#include <entt/signal/delegate.hpp>
#include <entt/core/hashed_string.hpp>
using namespace entt::literals;

#ifndef DEBUG
#define GLM_FORCE_INLINE
#endif
#define GLM_FORCE_INTRINSICS
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

using Scalar = float;

namespace serval {

    class Id {
    public:
        using type = entt::hashed_string::hash_type;
        static constexpr type INVALID = 0; 
        Id() = default;
        constexpr Id(type value) : m_value{value} {}
        constexpr Id(const entt::hashed_string& value) : m_value{value.value()} {}
        Id& operator= (type value) {
            m_value = value;
            return *this;
        }
        Id& operator= (entt::hashed_string value) {
            m_value = value.value();
            return *this;
        }
        operator type () const { return m_value; }
        type operator() () const { return m_value; }
        type operator* () const { return m_value; }

        bool valid () const { return m_value != INVALID; }
        
        constexpr static Id from (const std::string& str) {
            return {entt::hashed_string{str.c_str()}};
        }
    private:
        type m_value;
    };
    static_assert(std::is_trivial_v<Id>, "serval::Id must be trivial");
    static_assert(std::is_standard_layout_v<Id>, "serval::Id must be standard layout");

    enum class DataType : std::uint8_t {
        Invalid   = 0, // Value not found
        Byte      = 0b0001,
        Boolean   = 0b0010,
        Integer   = 0b0011,
        Integer64 = 0b0100,
        Scalar    = 0b0101,
        Entity    = 0b0110,
        Id        = 0b0111,
        Vec2      = 0b1000,
        Vec3      = 0b1001,
        Vec4      = 0b1010,
        Rotation  = 0b1011,
        Reserved1 = 0b1100,
        Reserved2 = 0b1101,
        Container = 0b1110,
        Handle    = 0b1111, // Any other type is abstracted behind a handle
    };
    using DataTypeID = typename std::underlying_type_t<DataType>;

    enum class Handle : std::uint32_t {};

    enum class ContainerType : std::uint8_t {
        Invalid   = 0b00,
        EnittySet = 0b01,
        List      = 0b10,
        Struct    = 0b11,
    };

    class ContainerHandle {
    public:
        ContainerHandle() = default;

        /**
         * @brief Return the type of container this handle represents
         * 
         * @return ContainerType 
         */
        ContainerType container_type () const {
            return static_cast<ContainerType>((m_packed_data >> 30) & 0x3);
        }

        /**
         * @brief Return the data type contained in this container
         * EntitySet always contains DataType::Entity
         * List cannot contain containers, handles or invalid
         * Struct and invalid do not contain types
         * 
         * @return DataType 
         */
        DataType contained_type () const {
            const auto container = container_type();
            if (container == ContainerType::EnittySet) {
                return DataType::Entity;
            } else if (container == ContainerType::List) {
                const auto type = static_cast<DataType>((m_packed_data >> 26) & 0x7);
                if ((type == DataType::Container | type == DataType::Handle | type == DataType::Invalid) == 0) {
                    return type;
                }
            }
            return DataType::Invalid;
        }

        /**
         * @brief Get the index of the container
         * 
         * @return std::uint32_t 
         */
        std::uint32_t index () const {
            return m_packed_data & 0x3ffffff;
        }
    private:
        std::uint32_t m_packed_data;
    };
    

    struct ParametersBuffer {
        std::byte* buffer;
        std::uint32_t metadata;
    };

    template <typename Type> struct WrapPtr { Type* ptr; };

    // IO types
    class Reader;
    class Writer;
    class Text;

    // Event callback types
    class StateEvents;
    class SystemEvents;
    class ExtensionEvents;

    // Runtime API type
    class Runtime;

    class Timeline;

    // Factory functions
    template <typename Class>
    struct FactoryFn {
        std::size_t size;
        Class*(*fn)(void*);
        Class* operator()(void* vp) const {
            return fn(vp);
        }
    };
    static_assert(std::is_trivial_v<FactoryFn<int>>, "FactoryFn<T> must be trivial");
    static_assert(std::is_trivial_v<FactoryFn<int>>, "FactoryFn<T> must be standard layout");
    template <typename Derived>
    struct makeFactoryFn {
        template <typename Class>
        operator FactoryFn<Class>() const {
            static_assert(std::is_base_of<Class, Derived>::value);
            return FactoryFn<Class>{ sizeof(Derived), +[](void* ptr) -> Class* { return new(ptr) Derived; } };
        }
    };

    // Callbacks
    using Task = entt::delegate<void(serval::Runtime&, Scalar)>;
    using AsyncTask = entt::delegate<void(serval::Runtime&)>;
    using Callback = entt::delegate<void()>;
    using EntityConstructor = entt::delegate<void(serval::Runtime&, entt::handle)>;
}

#endif