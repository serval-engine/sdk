#ifndef SERVAL_SDK__MACROS_HPP
#define SERVAL_SDK__MACROS_HPP

#include <hedley.h>

// Compiler optimizations

#define EXPECT_TAKEN(cond) (HEDLEY_LIKELY(cond))
#define EXPECT_NOT_TAKEN(cond) (HEDLEY_UNLIKELY(cond))
#if defined(HEDLEY_MSVC_VERSION)
#  define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name
#  define END_PACKED_STRUCT  __pragma(pack(pop))
#elif HEDLEY_HAS_ATTRIBUTE(packed)
#  define PACKED_STRUCT(name) struct __attribute__((packed)) name
#  define END_PACKED_STRUCT
#endif

// Silence warnings
#define MAYBE_UNUSED(var) (void)var

// Constraints

// TODO: _REQUIRE_IMPL to call error function instead of throwing

#define _REQUIRE_IMPL(fmtstr, ...) throw std::logic_error(fmt::format(fmtstr __VA_OPT__(,) __VA_ARGS__));
#define _ASSERT_IMPL(fmtstr, ...) {SPDLOG_ERROR(fmtstr __VA_OPT__(,) __VA_ARGS__); std::abort(-1);}

#define REQUIRE(condition, ...) {if EXPECT_NOT_TAKEN(!(condition)) {_REQUIRE_IMPL(__FILE__ ":" HEDLEY_STRINGIFY(__LINE__) " Requirement not met (" #condition "): " __VA_ARGS__)}}
#define FAIL(fmt, ...) _REQUIRE_IMPL((__FILE__ ":" HEDLEY_STRINGIFY(__LINE__) " " fmt)  __VA_OPT__(,) __VA_ARGS__)

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#define SOFT_REQUIRE(condition, ...) {if EXPECT_NOT_TAKEN(!(condition)){SPDLOG_WARN(__FILE__ ":" HEDLEY_STRINGIFY(__LINE__) " Requirement not met (" #condition "): " __VA_ARGS__)}}
#else
// Don't even bother checking if we can't warn
#define SOFT_REQUIRE(condition, ...)
#endif

#ifdef DEBUG
#define ASSERT(condition, ...) {if EXPECT_NOT_TAKEN(!(condition)){_ASSERT_IMPL(__FILE__ ":" HEDLEY_STRINGIFY(__LINE__) " Assertion failure (" #condition "): " __VA_ARGS__)}}
#else
#define ASSERT(condition, ...) (void)0
#endif

// Declaring types with static names and/or IDs

#define serval__ID_(type, name) static constexpr serval::Id HEDLEY_CONCAT(type, Id) = HEDLEY_CONCAT(name, _hs)
#define serval__NAME_(type, name) static constexpr char* HEDLEY_CONCAT(type, Name) = name
#define serval__TYPE_ID_(type, name) serval__ID_(HEDLEY_CONCAT(type, Type), name)

#define ASSET(name) serval__TYPE_ID_(Asset, name)
#define COMPONENT(name) serval__NAME_(Component, name); serval__TYPE_ID_(Component, name)
#define SYSTEM(name) serval__NAME_(System, name); serval__ID_(System, name)

// Create member checker templates
#define MAKE_HAS_MEMBER(member) \
    template <typename T> \
    struct has_##member { \
    private: \
        template <typename U> static std::true_type test(decltype(U::member)*); \
        template <typename U> static std::false_type test(...); \
    public: \
        static constexpr bool value = decltype(test<T>(nullptr))::value; \
    }

#endif