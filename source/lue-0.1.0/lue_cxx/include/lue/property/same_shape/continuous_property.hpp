#pragma once
#include "lue/property_set.hpp"
#include "lue/item/same_shape/continuous_value.hpp"


namespace lue {
namespace same_shape {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .
*/
class ContinuousProperty:
    public Property
{

public:

                   ContinuousProperty  (hdf5::Identifier const& id);

                   ContinuousProperty  (hdf5::Group& parent,
                                        std::string const& name);

                   ContinuousProperty  (ContinuousProperty const&)=delete;

                   ContinuousProperty  (ContinuousProperty&&)=default;

                   ~ContinuousProperty ()=default;

    ContinuousProperty&
                   operator=           (ContinuousProperty const&)=delete;

    ContinuousProperty&
                   operator=           (ContinuousProperty&&)=default;

    ContinuousValue&
                   value               ();

private:

    ContinuousValue _value;

};


ContinuousProperty create_continuous_property(
                                        PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

ContinuousProperty create_continuous_property(
                                        PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace lue
