#pragma once
#include "wildfire_model_base.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {

class WildfireBenchmarkModel final:
    public WildfireModelBase
{

public:

                   WildfireBenchmarkModel(
                                        benchmark::Environment const& environment,
                                        benchmark::Task const& task);

                   WildfireBenchmarkModel(
                                        WildfireBenchmarkModel const&)=default;

                   WildfireBenchmarkModel(
                                        WildfireBenchmarkModel&&)=default;

                   ~WildfireBenchmarkModel()=default;

    WildfireBenchmarkModel&
                   operator=           (WildfireBenchmarkModel const&)=default;

    WildfireBenchmarkModel&
                   operator=           (WildfireBenchmarkModel&&)=default;

protected:

private:

    BooleanRaster   initial_fire       () const final;

    RasterShape    _raster_shape;

    RasterShape    _partition_shape;

    BooleanRaster  _clone;

};

}  // namespace lue
