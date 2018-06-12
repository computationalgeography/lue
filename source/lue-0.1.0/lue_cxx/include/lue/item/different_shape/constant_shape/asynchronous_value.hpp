#pragma once
#include "lue/item/different_shape/constant_shape/value.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - Different number of different-shaped values per item
*/
class AsynchronousValue:
    public Value
{

public:

                   AsynchronousValue   (hdf5::Group const& parent,
                                        std::string const& name);

                   AsynchronousValue   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousValue   (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousValue   (AsynchronousValue const&)=delete;

                   AsynchronousValue   (AsynchronousValue&&)=default;

                   ~AsynchronousValue  ()=default;

    AsynchronousValue&
                   operator=           (AsynchronousValue const&)=delete;

    AsynchronousValue&
                   operator=           (AsynchronousValue&&)=default;

private:

};


AsynchronousValue create_asynchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

AsynchronousValue create_asynchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue
