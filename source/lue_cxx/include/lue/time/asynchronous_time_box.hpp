#pragma once
#include "lue/item/same_shape/constant_shape/asynchronous_value.hpp"
#include "lue/clock.hpp"


namespace lue {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - Zero or more time boxes per item
    - Each time box has a unique location in time
*/
class AsynchronousTimeBox:
    public same_shape::constant_shape::AsynchronousValue
{

public:

                   AsynchronousTimeBox (hdf5::Group const& parent);

                   AsynchronousTimeBox (AsynchronousTimeBox const&)=delete;

                   AsynchronousTimeBox (AsynchronousTimeBox&&)=default;

                   ~AsynchronousTimeBox()=default;

    AsynchronousTimeBox&
                   operator=           (AsynchronousTimeBox const&)=delete;

    AsynchronousTimeBox&
                   operator=           (AsynchronousTimeBox&&)=default;

private:

};


AsynchronousTimeBox create_asynchronous_time_box(
                                        hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
