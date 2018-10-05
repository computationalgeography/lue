#pragma once
#include "lue/object/property/same_shape/properties.hpp"
#include "lue/object/property/same_shape/constant_shape/properties.hpp"
#include "lue/object/property/same_shape/variable_shape/properties.hpp"
#include "lue/object/property/different_shape/properties.hpp"
#include "lue/object/property/different_shape/constant_shape/properties.hpp"
#include "lue/object/property/different_shape/variable_shape/properties.hpp"
#include "lue/object/identity/object_tracker.hpp"


namespace lue {

class Properties:
    public hdf5::Group
{

public:

    explicit       Properties          (hdf5::Group& parent);

                   Properties          (hdf5::Group&& group);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

    bool           empty               () const;

    std::size_t    size                () const;

    // std::vector<std::string>
    //                names               () const;

    bool           contains            (std::string const& name) const;

    ShapePerObject shape_per_object    (std::string const& name) const;

    ValueVariability value_variability (std::string const& name) const;

    ShapeVariability shape_variability (std::string const& name) const;

    template<
        typename T>
    T const&       collection          () const;

    template<
        typename T>
    T&             collection          ();

    same_shape::Property& add          (std::string const& name,
                                        hdf5::Datatype const& datatype);

    same_shape::Property& add          (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        hdf5::Shape const& shape);

    different_shape::Property&
                   add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        Rank rank);

    template<
        typename Property>
    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype);

    template<
        typename Property>
    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        hdf5::Shape const& shape);

    template<
        typename Property>
    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        Rank rank);

private:

    void           verify_property_does_not_exist(
                                        std::string const& name) const;

    same_shape::Properties _same_shape_properties;

    same_shape::constant_shape::Properties
        _same_shape_constant_shape_properties;

    same_shape::variable_shape::Properties
        _same_shape_variable_shape_properties;

    different_shape::Properties _different_shape_properties;

    different_shape::constant_shape::Properties
        _different_shape_constant_shape_properties;

    different_shape::variable_shape::Properties
        _different_shape_variable_shape_properties;

};


Properties         create_properties   (hdf5::Group& parent,
                                        ObjectTracker& object_tracker);


#define COLLECTION(type, member)                         \
template<>                                               \
inline type const& Properties::collection<type>() const  \
{                                                        \
    return member;                                       \
}                                                        \
                                                         \
                                                         \
template<>                                               \
inline type& Properties::collection<type>()              \
{                                                        \
    return member;                                       \
}


COLLECTION(
    same_shape::Properties,
    _same_shape_properties)
COLLECTION(
    same_shape::constant_shape::Properties,
    _same_shape_constant_shape_properties)
COLLECTION(
    same_shape::variable_shape::Properties,
    _same_shape_variable_shape_properties)
COLLECTION(
    different_shape::Properties,
    _different_shape_properties)
COLLECTION(
    different_shape::constant_shape::Properties,
    _different_shape_constant_shape_properties)
COLLECTION(
    different_shape::variable_shape::Properties,
    _different_shape_variable_shape_properties)

#undef COLLECTION


template<>
inline same_shape::constant_shape::Property&
        Properties::add<same_shape::constant_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    verify_property_does_not_exist(name);

    return _same_shape_constant_shape_properties.add(name, datatype);
}


template<>
inline same_shape::Property&
        Properties::add<same_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype,
    hdf5::Shape const& shape)
{
    verify_property_does_not_exist(name);

    return _same_shape_properties.add(name, datatype, shape);
}


template<>
inline same_shape::constant_shape::Property&
        Properties::add<same_shape::constant_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype,
    hdf5::Shape const& shape)
{
    verify_property_does_not_exist(name);

    return _same_shape_constant_shape_properties.add(name, datatype, shape);
}


template<>
inline same_shape::variable_shape::Property&
        Properties::add<same_shape::variable_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank)
{
    verify_property_does_not_exist(name);

    return _same_shape_variable_shape_properties.add(name, datatype, rank);
}


template<>
inline different_shape::constant_shape::Property&
        Properties::add<different_shape::constant_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank)
{
    verify_property_does_not_exist(name);

    return _different_shape_constant_shape_properties.add(
        name, datatype, rank);
}


template<>
inline different_shape::variable_shape::Property&
        Properties::add<different_shape::variable_shape::Property>(
    std::string const& name,
    hdf5::Datatype const& datatype,
    Rank const rank)
{
    verify_property_does_not_exist(name);

    return _different_shape_variable_shape_properties.add(
        name, datatype, rank);
}

}  // namespace lue
