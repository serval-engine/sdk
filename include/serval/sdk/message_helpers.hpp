#ifndef SERVAL_SDK__MESSAGE_HELPERS_HPP
#define SERVAL_SDK__MESSAGE_HELPERS_HPP

#include "types.hpp"
#include "variant.hpp"

namespace msghelpers {
    // Unpack the vararg parameters and generate a series of 3bit TypeID fields, packed into bits 4...17 (inclusive) of the 32bit metadata
    template <std::uint32_t Metadata, int Index, typename Param, typename... Params>
    constexpr std::uint32_t generate_metadata () {
        constexpr auto ValueType = serval::variant::type_of<Param>();
        static_assert(ValueType != serval::variant::Type::Invalid, "Message parameter has an incompatible type");
        constexpr std::uint32_t metadata = Metadata | (serval::variant::to_id(ValueType) << (3 + (Index * 3)));
        if constexpr (sizeof...(Params) > 0) {
            // Advance the index of the parameter and recursively unpack the remaining parameters
            return generate_metadata<metadata, Index + 1, Params...>();
        } else {
            // Finally return the fully formed metadata
            return metadata;
        }
    }

    // Copy the vararg parameters into the buffer
    template <typename Param, typename... Params>
    void populate_parameters_buffer (std::byte* buffer, Param&& param, Params&&... params) {
        static_assert(std::is_trivial_v<Param>, "Message parameter is not trivially copyable");
        new (buffer) Param{std::forward<Param>(param)};
        if constexpr (sizeof...(params) > 0) {
            // Advance the buffer forward by the size of the newly copied parameter and recursively copy remaining parameters
            populate_parameters_buffer(buffer + sizeof(Param), std::forward<Params>(params)...);
        }
    }
}

#endif