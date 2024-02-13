#ifndef SERVAL_SDK__TYPE_UTILS_HPP
#define SERVAL_SDK__TYPE_UTILS_HPP

#include <type_traits>

namespace serval {
    template <typename T> struct class_of;
    template <typename C, typename R>
    struct class_of<R C::*> {
        using type = C;
    };
    template <typename T> using class_of_t = typename class_of<T>::type;

    template <typename, typename = std::void_t<>>
    struct is_resource : std::false_type {};
    template <typename T>
    struct is_resource<T, std::void_t<decltype(std::declval<T>().ResourceTypeID)>> : std::true_type {};

    template <typename, typename = std::void_t<>>
    struct is_component : std::false_type {};
    template <typename T>
    struct is_component<T, std::void_t<decltype(std::declval<T>().ComponentTypeID)>> : std::true_type {};

    template <typename, typename = std::void_t<>>
    struct is_system : std::false_type {};
    template <typename T>
    struct is_system<T, std::void_t<decltype(std::declval<T>().SystemID)>> : std::true_type {};

    template <typename T>
    constexpr bool is_resource_v = is_resource<T>::value;

    template <typename T>
    constexpr bool is_component_v = is_component<T>::value;

    template <typename T>
    constexpr bool is_system_v = is_system<T>::value;
}

#endif