#pragma once
#include "lue/item/same_shape/constant_shape/continuous_value.hpp"
#include "lue/property_set.hpp"


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

                   // ContinuousProperty  (hdf5::Identifier const& id);

                   ContinuousProperty  (hdf5::Group& parent,
                                        std::string const& name);

                   ContinuousProperty  (ContinuousProperty const&)=delete;

                   ContinuousProperty  (ContinuousProperty&&)=default;

                   ~ContinuousProperty ()=default;

    ContinuousProperty&
                   operator=           (ContinuousProperty const&)=delete;

    ContinuousProperty&
                   operator=           (ContinuousProperty&&)=default;

    constant_shape::ContinuousValue&
                   value               ();

private:

    constant_shape::ContinuousValue _value;

};


ContinuousProperty create_continuous_property(
                                        PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

}  // namespace same_shape
}  // namespace lue
