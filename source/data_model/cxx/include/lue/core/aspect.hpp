#pragma once
#include "lue/core/define.hpp"
#include "lue/core/enum_string_bimap.hpp"
#include "lue/core/tag.hpp"
#include "lue/core/time.hpp"
#include "lue/hdf5/attributes.hpp"
#include <fmt/format.h>


namespace lue {

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

TAG_AND_MAP(ShapePerObject, shape_per_object_tag, ({
    { ShapePerObject::same, "same_shape" },
    { ShapePerObject::different, "different_shape" }
}))

TAG_AND_MAP(ValueVariability, value_variability_tag, ({
    { ValueVariability::constant, "constant" },
    { ValueVariability::variable, "variable" }
}))

TAG_AND_MAP(ShapeVariability, shape_variability_tag, ({
    { ShapeVariability::constant, "constant_shape" },
    { ShapeVariability::variable, "variable_shape" }
}))

TAG_AND_MAP(TimeDomainItemType, time_domain_item_type_tag, ({
    { TimeDomainItemType::box, "box" },
    { TimeDomainItemType::cell, "cell" },
    { TimeDomainItemType::point, "point" }
}))

TAG_AND_MAP(Mobility, mobility_tag, ({
    { Mobility::mobile, "mobile" },
    { Mobility::stationary, "stationary" }
}))

TAG_AND_MAP(SpaceDomainItemType, space_domain_item_type_tag, ({
    { SpaceDomainItemType::point, "point" },
    { SpaceDomainItemType::box, "box" }
}))

TAG_AND_MAP(SpaceDiscretization, space_discretization_tag, ({
    { SpaceDiscretization::regular_grid, "regular_grid" }
}))

TAG_AND_MAP(TimeDiscretization, time_discretization_tag, ({
    { TimeDiscretization::regular_grid, "regular_grid" }
}))

TAG_AND_MAP(time::Unit, time_unit_tag, ({
    { time::Unit::second, time::UnitTraits<time::Unit::second>::name() },
    { time::Unit::minute, time::UnitTraits<time::Unit::minute>::name() },
    { time::Unit::hour, time::UnitTraits<time::Unit::hour>::name() },
    { time::Unit::day, time::UnitTraits<time::Unit::day>::name() },
    { time::Unit::week, time::UnitTraits<time::Unit::week>::name() },
    { time::Unit::month, time::UnitTraits<time::Unit::month>::name() },
    { time::Unit::year, time::UnitTraits<time::Unit::year>::name() }
}))

TAG_AND_MAP(time::Calendar, epoch_calendar_tag, ({
    { time::Calendar::gregorian, "gregorian"}
}))

TAG_AND_MAP(time::Epoch::Kind, epoch_kind_tag, ({
    { time::Epoch::Kind::anno_domini, "anno_domini"},
    { time::Epoch::Kind::formation_of_earth, "formation_of_earth"}
}))

#undef TAG_AND_MAP
#undef MAP
#undef TAG


template<
    typename T>
inline std::string aspect_to_string(
    T const value)
{
    return AspectMap<T>::value().as_string(value);
}


template<
    typename T>
inline T string_to_aspect(
    std::string const& string)
{
    auto const& map = AspectMap<T>::value();

    if(!map.contains(string)) {
        throw std::runtime_error(fmt::format(
                "Unknown aspect: {}",
                string
            ));
    }

    return map.as_value(string);
}


template<
    typename T>
class Aspect
{

public:

    explicit       Aspect              (T value);

    explicit       Aspect              (hdf5::Attributes const& attributes);

                   Aspect              (Aspect const&)=default;

                   Aspect              (Aspect&&)=default;

                   ~Aspect             ()=default;

    Aspect&        operator=           (Aspect const&)=default;

    Aspect&        operator=           (Aspect&&)=default;

    T              value               () const;

    void           save                (hdf5::Attributes& attributes) const;

    friend bool operator==(
        Aspect<T> const& lhs,
        Aspect<T> const& rhs)
    {
        return lhs._value == rhs._value;
    }

protected:

    void           load                (hdf5::Attributes const& attributes);

private:

    T              _value;

};


template<
    typename T>
inline Aspect<T>::Aspect(
    T const value)

    : _value{value}
{
}


/*!
    @brief      Read configuration aspect from @a attributes
    @sa         Tag, string_to_aspect(std::string const&)

    The template parameter @a T is used to lookup a tag (a string),
    which is used as the name of the attribute to read. The attribute value
    is translated into an instance of @a T.
*/
template<
    typename T>
inline Aspect<T>::Aspect(
    hdf5::Attributes const& attributes)

    : _value{string_to_aspect<T>(
        attributes.read<std::string>(Tag<T>::value()))}

{
}


template<
    typename T>
inline T Aspect<T>::value() const
{
    return _value;
}


template<
    typename T>
inline void Aspect<T>::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        Tag<T>::value(), aspect_to_string<T>(_value));
}


template<
    typename T>
inline void Aspect<T>::load(
    hdf5::Attributes const& attributes)
{
    _value = string_to_aspect<T>(
        attributes.read<std::string>(Tag<T>::value()));
}

}  // namespace lue
