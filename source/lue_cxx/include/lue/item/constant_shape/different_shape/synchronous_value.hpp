#pragma once
#include "lue/item/constant_shape/different_shape/value.hpp"


namespace lue {
namespace constant_shape {
namespace different_shape {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .
*/
class SynchronousValue:
    public Value
{

public:

                   SynchronousValue    (hdf5::Group const& parent,
                                        std::string const& name);

                   SynchronousValue    (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousValue    (SynchronousValue const&)=delete;

                   SynchronousValue    (SynchronousValue&&)=default;

                   ~SynchronousValue   ()=default;

    SynchronousValue&
                   operator=           (SynchronousValue const&)=delete;

    SynchronousValue&
                   operator=           (SynchronousValue&&)=default;

private:

};


SynchronousValue   create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

SynchronousValue   create_synchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace lue
