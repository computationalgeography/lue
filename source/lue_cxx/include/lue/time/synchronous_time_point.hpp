#pragma once
#include "lue/time/synchronous_time_domain_item.hpp"
#include "lue/clock.hpp"


namespace lue {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - Zero or more time points for all items
    - Each time point has a unique location in time
*/
class SynchronousTimePoint:
    public SynchronousTimeDomainItem
{

public:

                   SynchronousTimePoint(hdf5::Group& parent);

                   SynchronousTimePoint(SynchronousTimePoint const&)=delete;

                   SynchronousTimePoint(SynchronousTimePoint&&)=default;

                   ~SynchronousTimePoint()=default;

    SynchronousTimePoint&
                   operator=           (SynchronousTimePoint const&)=delete;

    SynchronousTimePoint&
                   operator=           (SynchronousTimePoint&&)=default;

private:

};


SynchronousTimePoint create_synchronous_time_point(
                                        hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
