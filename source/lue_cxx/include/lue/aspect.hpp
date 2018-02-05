#pragma once
#include "lue/define.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"
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


template<>
struct Tag<CollectionVariability>
{
    static std::string const& value() {
        static std::string const tag = collection_variability_tag;
        return tag;
    }
};


template<>
struct AspectMap<CollectionVariability>
{
    static detail::EnumStringBimap<CollectionVariability> const& value()
    {
        static detail::EnumStringBimap<CollectionVariability> const map {
            { CollectionVariability::constant, "lue_constant_collection" }
        };

        return map;
    }
};


template<>
struct Tag<ShapeVariability>
{
    static std::string const& value() {
        static std::string const tag = shape_variability_tag;
        return tag;
    }
};


template<>
struct AspectMap<ShapeVariability>
{
    static detail::EnumStringBimap<ShapeVariability> const& value()
    {
        static detail::EnumStringBimap<ShapeVariability> const map {
            { ShapeVariability::constant, "lue_constant_shape" }
        };

        return map;
    }
};


template<>
struct Tag<ShapePerItem>
{
    static std::string const& value() {
        static std::string const tag = shape_per_item_tag;
        return tag;
    }
};


template<>
struct AspectMap<ShapePerItem>
{
    static detail::EnumStringBimap<ShapePerItem> const& value()
    {
        static detail::EnumStringBimap<ShapePerItem> const map {
            { ShapePerItem::same, "lue_same_shape" },
            { ShapePerItem::different, "lue_different_shape" },
        };

        return map;
    }
};


template<>
struct Tag<ValueVariability>
{
    static std::string const& value() {
        static std::string const tag = value_variability_tag;
        return tag;
    }
};


template<>
struct AspectMap<ValueVariability>
{
    static detail::EnumStringBimap<ValueVariability> const& value()
    {
        static detail::EnumStringBimap<ValueVariability> const map {
            { ValueVariability::constant, "lue_constant_value" },
            { ValueVariability::variable, "lue_variable_value" },
        };

        return map;
    }
};


template<>
struct Tag<Mobility>
{
    static std::string const& value() {
        static std::string const tag = mobility_tag;
        return tag;
    }
};


template<>
struct AspectMap<Mobility>
{
    static detail::EnumStringBimap<Mobility> const& value()
    {
        static detail::EnumStringBimap<Mobility> const map {
            // { Mobility::mobile, "lue_mobile" },
            { Mobility::stationary, "lue_stationary" },
        };

        return map;
    }
};


template<>
struct Tag<SpaceDomainItemType>
{
    static std::string const& value() {
        static std::string const tag = space_domain_item_type_tag;
        return tag;
    }
};


template<>
struct AspectMap<SpaceDomainItemType>
{
    static detail::EnumStringBimap<SpaceDomainItemType> const& value()
    {
        static detail::EnumStringBimap<SpaceDomainItemType> const map {
            { SpaceDomainItemType::point, "lue_point" },
            { SpaceDomainItemType::box, "lue_box" },
        };

        return map;
    }
};


template<>
struct Tag<TimeDomainItemType>
{
    static std::string const& value() {
        static std::string const tag = time_domain_item_type_tag;
        return tag;
    }
};


template<>
struct AspectMap<TimeDomainItemType>
{
    static detail::EnumStringBimap<TimeDomainItemType> const& value()
    {
        static detail::EnumStringBimap<TimeDomainItemType> const map {
            { TimeDomainItemType::point, "lue_point" },
            { TimeDomainItemType::box, "lue_box" },
        };

        return map;
    }
};

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
