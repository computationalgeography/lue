#pragma once
#include <cstdint>
#include <string>


namespace lue {
namespace framework {

template<
    typename Element>
class TypeTraits
{};


#define TYPE_TRAITS(Element, name_)                   \
template<>                                            \
class TypeTraits<Element>                             \
{                                                     \
                                                      \
    public:                                           \
                                                      \
        inline static std::string const name{name_};  \
                                                      \
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

}  // namespace framework
}  // namespace lue
