#pragma once
#include "lue/define.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"
// #include "lue/time/unit.hpp"
#include "lue/hdf5/attributes.hpp"


namespace lue {
namespace detail {

template<
    typename T>
struct Tag
{
    static std::string const& value();
};


template<
    typename T>
struct AspectMap
{
    static detail::EnumStringBimap<T> const& value();
};


#define TAG(EnumType, tag_name)                   \
template<>                                        \
struct Tag<EnumType>                              \
{                                                 \
    static std::string const& value() {           \
        static std::string const tag = tag_name;  \
        return tag;                               \
    }                                             \
};


#define MAP(EnumType, enum_pairs)                            \
template<>                                                   \
struct AspectMap<EnumType>                                   \
{                                                            \
    static detail::EnumStringBimap<EnumType> const& value()  \
    {                                                        \
        static detail::EnumStringBimap<EnumType> const map   \
            enum_pairs                                       \
        ;                                                    \
                                                             \
        return map;                                          \
    }                                                        \
};


#define TAG_AND_MAP(EnumType, tag_name, enum_pairs)  \
TAG(EnumType, tag_name)                              \
MAP(EnumType, enum_pairs)

TAG_AND_MAP(Occurrence, occurrence_tag, ({
    { Occurrence::continuous, "lue_continuous" },
    { Occurrence::asynchronous, "lue_asynchronous" },
    { Occurrence::synchronous, "lue_synchronous" }
}))

TAG_AND_MAP(CollectionVariability, collection_variability_tag, ({
    { CollectionVariability::constant, "lue_constant_collection" }
}))

TAG_AND_MAP(ShapeVariability, shape_variability_tag, ({
    { ShapeVariability::constant, "lue_constant_shape" }
}))

TAG_AND_MAP(ShapePerItem, shape_per_item_tag, ({
    { ShapePerItem::same, "lue_same_shape" },
    { ShapePerItem::different, "lue_different_shape" }
}))

TAG_AND_MAP(Mobility, mobility_tag, ({
    // { Mobility::mobile, "lue_mobile" },
    { Mobility::stationary, "lue_stationary" }
}))

TAG_AND_MAP(SpaceDomainItemType, space_domain_item_type_tag, ({
    { SpaceDomainItemType::point, "lue_point" },
    { SpaceDomainItemType::box, "lue_box" }
}))

TAG_AND_MAP(TimeDomainItemType, time_domain_item_type_tag, ({
    { TimeDomainItemType::point, "lue_point" },
    { TimeDomainItemType::box, "lue_box" }
}))

// TAG_AND_MAP(time::Unit, unit_tag, ({
//     { time::Unit::second, "lue_second" }
// }))

#undef TAG_AND_MAP
#undef MAP
#undef TAG

}  // namespace detail


template<
    typename T>
inline std::string aspect_to_string(
    T const type)
{
    return detail::AspectMap<T>::value().as_string(type);
}


template<
    typename T>
inline T string_to_aspect(
    std::string const& string)
{
    auto const& map = detail::AspectMap<T>::value();

    if(!map.contains(string)) {
        throw std::runtime_error("Unknown aspect: " + string);
    }

    return map.as_value(string);
}


template<
    typename T>
class Aspect
{

public:

    explicit       Aspect              (T type);

    explicit       Aspect              (hdf5::Attributes const& attributes);

                   Aspect              (Aspect const&)=default;

                   Aspect              (Aspect&&)=default;

                   ~Aspect             ()=default;

    Aspect&        operator=           (Aspect const&)=default;

    Aspect&        operator=           (Aspect&&)=default;

    T              type                () const;

    void           save                (hdf5::Attributes& attributes) const;

protected:

    void           load                (hdf5::Attributes const& attributes);

private:

    T              _type;

};


template<
    typename T>
inline Aspect<T>::Aspect(
    T const type)

    : _type{type}
{
}


template<
    typename T>
inline Aspect<T>::Aspect(
    hdf5::Attributes const& attributes)

    : _type{string_to_aspect<T>(
        attributes.read<std::string>(detail::Tag<T>::value()))}

{
}


template<
    typename T>
inline T Aspect<T>::type() const
{
    return _type;
}


template<
    typename T>
inline void Aspect<T>::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        detail::Tag<T>::value(), aspect_to_string<T>(_type));
}


template<
    typename T>
inline void Aspect<T>::load(
    hdf5::Attributes const& attributes)
{
    _type = string_to_aspect<T>(
        attributes.read<std::string>(detail::Tag<T>::value()));
}

}  // namespace lue
