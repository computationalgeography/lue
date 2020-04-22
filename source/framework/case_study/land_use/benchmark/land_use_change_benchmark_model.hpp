#pragma once
#include "land_use_change_model_base.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"


namespace lue {

class LandUseChangeBenchmarkModel final:
    public LandUseChangeModelBase
{

public:

    // using Base = LandUseChangeModelBase;

    // using Base::NominalRaster;

    // using Base::ScalarRaster;

                   LandUseChangeBenchmarkModel(
                                        benchmark::Environment const& environment,
                                        benchmark::Task const& task);

                   LandUseChangeBenchmarkModel(
                                        LandUseChangeBenchmarkModel const&)=default;

                   LandUseChangeBenchmarkModel(
                                        LandUseChangeBenchmarkModel&&)=default;

                   ~LandUseChangeBenchmarkModel()=default;

    LandUseChangeBenchmarkModel&
                   operator=           (LandUseChangeBenchmarkModel const&)=default;

    LandUseChangeBenchmarkModel&
                   operator=           (LandUseChangeBenchmarkModel&&)=default;

protected:

private:

    NominalRaster  land_use            () const final;

    ScalarRaster   elevation           () const final;

    RasterShape    _raster_shape;

    RasterShape    _partition_shape;

};

}  // namespace lue
