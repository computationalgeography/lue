#pragma once
#include "lue/item/same_shape/continuous_value.hpp"


namespace lue {
namespace stationary {

class Box:
    public same_shape::ContinuousValue
{

public:

                   Box                 (hdf5::Group const& parent);

                   Box                 (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   Box                 (Box const&)=delete;

                   Box                 (Box&&)=default;

                   ~Box                ()=default;

    Box&           operator=           (Box const&)=delete;

    Box&           operator=           (Box&&)=default;

private:

};


Box                create_box          (hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

Box                create_box          (hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace stationary
}  // namespace lue
