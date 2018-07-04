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

    bool           contains            (std::string const& name) const;

    ShapePerObject shape_per_object    (std::string const& name) const;

    ValueVariability value_variability (std::string const& name) const;

    ShapeVariability shape_variability (std::string const& name) const;

    template<
        typename T>
    T const&       collection          () const;

    same_shape::Property& add          (std::string const& name,
                                        hdf5::Datatype const& datatype);

private:

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


template<>
inline same_shape::Properties const&
    Properties::collection<same_shape::Properties>() const
{
    return _same_shape_properties;
}


template<>
inline same_shape::constant_shape::Properties const&
    Properties::collection<same_shape::constant_shape::Properties>() const
{
    return _same_shape_constant_shape_properties;
}


template<>
inline same_shape::variable_shape::Properties const&
    Properties::collection<same_shape::variable_shape::Properties>() const
{
    return _same_shape_variable_shape_properties;
}


template<>
inline different_shape::Properties const&
    Properties::collection<different_shape::Properties>() const
{
    return _different_shape_properties;
}


template<>
inline different_shape::constant_shape::Properties const&
    Properties::collection<different_shape::constant_shape::Properties>() const
{
    return _different_shape_constant_shape_properties;
}


template<>
inline different_shape::variable_shape::Properties const&
    Properties::collection<different_shape::variable_shape::Properties>() const
{
    return _different_shape_variable_shape_properties;
}

}  // namespace lue
