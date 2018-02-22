#pragma once
#include "lue/item/same_shape/constant_shape/asynchronous_value.hpp"
#include "lue/clock.hpp"


namespace lue {

class AsynchronousTimePoint:
    public same_shape::constant_shape::AsynchronousValue
{

public:

                   AsynchronousTimePoint(
                                        hdf5::Group const& parent);

                   AsynchronousTimePoint(
                                        AsynchronousTimePoint const&)=delete;

                   AsynchronousTimePoint(
                                        AsynchronousTimePoint&&)=default;

                   ~AsynchronousTimePoint()=default;

    AsynchronousTimePoint&
                   operator=           (AsynchronousTimePoint const&)=delete;

    AsynchronousTimePoint&
                   operator=           (AsynchronousTimePoint&&)=default;

private:

};


AsynchronousTimePoint create_asynchronous_time_point(
                                        hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
