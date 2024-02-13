#ifndef SERVAL_SDK__ATTRIBUTES_HPP_
#define SERVAL_SDK__ATTRIBUTES_HPP_

#include "../types.hpp"
#include "../variant.hpp"

namespace serval {
    class Attributes;
}

class serval::Attributes {
public:
    ASSET("attributes");

    virtual ~Attributes () {}

    template <typename T>
    [[nodiscard]] bool try_get (serval::Id key, T& out) const {
        constexpr auto ValueType = variant::type_of<T>();
        static_assert(ValueType != variant::Type::Invalid, "Tried to get Attributes value as an incompatible type");
        variant::ToType<T> temp; // Force T to compatible type
        if (read(key, variant::to_id(ValueType), &temp)) {
            out = temp;
            return true;
        }
        return false;
    }

    template <typename T>
    std::optional<T> operator[] (serval::Id key) const {
        constexpr auto ValueType = variant::type_of<T>();
        static_assert(ValueType != variant::Type::Invalid, "Tried to get Attributes value as an incompatible type");
        variant::ToType<T> temp; // Force T to compatible type
        if (read(key, variant::to_id(ValueType), &temp)) {
            return {T{temp}};
        } else {
            return {};
        }
    }

    bool contains (serval::Id key) const {
        return type(key) != variant::Type::Invalid;
    }

    template <typename T>
    bool has (serval::Id key) const {
        return get_type(key) == variant::to_id(variant::type_of<T>());
    }

    variant::Type type (serval::Id key) const {
        return variant::from_id(get_type(key));
    }

    template <typename T>
    bool set (serval::Id key, const T& value) {
        constexpr auto ValueType = variant::type_of<T>();
        static_assert(ValueType != variant::Type::Invalid, "Tried to set Attributes value with incompatible type");
        variant::ToType<T> temp{value}; // Force T to compatible type
        return write(key, variant::to_id(ValueType), &temp);
    }

    /**
     * @brief Update an attribute by calling a function with a reference to that attribute
     * attribs.update<mind::variant::Integer>("foo"_hs, [](auto& foo){ foo = 5; })
     * If key does not exist, default value is passed to function and new value is written.
     * If key exists, but is of a different type, ... TODO what then?
     *      * 
     * @tparam ValueType The type of the value to update
     * @tparam Func 
     * @param key 
     * @param func 
     * @return Attributes& 
     */
    template <variant::Type ValueType, typename Func>
    Attributes& update (serval::Id key, Func&& func) {
        static_assert(ValueType != variant::Type::Invalid, "Tried to update Attributes value as an incompatible type");
        constexpr auto id = variant::to_id(ValueType);
        variant::TypeOf<ValueType> temp; // Force T to compatible type
        if (contains(key)) {
            bool ok = read(key, id, &temp);
            // REQUIRE(ok, "Attributes::update(key, fn) type mismatch");
            if (ok) {
                func(temp);
                write(key, id, &temp);
            } else {
                // TODO: How should a type mismatch be handled?
                // TODO: Provide REQUIRE to SDK and expose an engine_abort(...) function to terminate engine with error
            }
        } else {
            write(key, id, &temp);
        }
        return *this;
    }

private:
    virtual variant::TypeId get_type (serval::Id key) const = 0;
    virtual bool read (serval::Id key, variant::TypeId type_id, void* out_value) const = 0;
    virtual bool write (serval::Id key, variant::TypeId type_id, const void* value) = 0;
};

#endif