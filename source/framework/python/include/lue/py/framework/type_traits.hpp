#pragma once
#include <pybind11/numpy.h>
#include <cstdint>
#include <string>


namespace lue::framework {

    template<typename Element>
    class TypeTraits
    {
    };


    template<>
    class TypeTraits<void>
    {

        public:

            inline static std::string const name{"void"};
    };


#define TYPE_TRAITS(Element, name_)                                                                          \
    template<>                                                                                               \
    class TypeTraits<Element>                                                                                \
    {                                                                                                        \
                                                                                                             \
        public:                                                                                              \
                                                                                                             \
            inline static std::string const name{name_};                                                     \
                                                                                                             \
            inline static pybind11::dtype dtype()                                                            \
            {                                                                                                \
                return pybind11::dtype::of<Element>();                                                       \
            }                                                                                                \
    };

    TYPE_TRAITS(std::uint8_t, "uint8")
    TYPE_TRAITS(std::uint32_t, "uint32")
    TYPE_TRAITS(std::uint64_t, "uint64")
    TYPE_TRAITS(std::int8_t, "int8")
    TYPE_TRAITS(std::int32_t, "int32")
    TYPE_TRAITS(std::int64_t, "int64")
    TYPE_TRAITS(float, "float32")
    TYPE_TRAITS(double, "float64")


#undef TYPE_TRAITS

    template<typename Element>
    auto as_string() -> std::string
    {
        return TypeTraits<Element>::name;
    }


    template<typename Element>
    auto dtype() -> pybind11::dtype
    {
        return TypeTraits<Element>::dtype();
    }

}  // namespace lue::framework
