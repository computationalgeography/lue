#pragma once
#include "lue/time/synchronous_time_domain_item.hpp"
#include "lue/clock.hpp"


namespace lue {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - Zero or more time boxes for all items
    - Each time box has a unique location in time
*/
class SynchronousTimeBox:
    public SynchronousTimeDomainItem
{

public:

                   SynchronousTimeBox  (hdf5::Group& parent);

                   SynchronousTimeBox  (SynchronousTimeBox const&)=delete;

                   SynchronousTimeBox  (SynchronousTimeBox&&)=default;

                   ~SynchronousTimeBox ()=default;

    SynchronousTimeBox&
                   operator=           (SynchronousTimeBox const&)=delete;

    SynchronousTimeBox&
                   operator=           (SynchronousTimeBox&&)=default;

private:

};


SynchronousTimeBox create_synchronous_time_box(
                                        hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
