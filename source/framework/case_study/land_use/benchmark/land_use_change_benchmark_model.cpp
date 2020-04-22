#include "land_use_change_benchmark_model.hpp"


namespace lue {

LandUseChangeBenchmarkModel::LandUseChangeBenchmarkModel(
    benchmark::Environment const& /* environment */,
    benchmark::Task const& task):

    LandUseChangeModelBase{},
    _raster_shape{},
    _partition_shape{}

{
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        _raster_shape.begin());

    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        _partition_shape.begin());
}


LandUseChangeBenchmarkModel::NominalRaster
    LandUseChangeBenchmarkModel::land_use() const
{
    NominalRaster land_use{_raster_shape, _partition_shape};

    return land_use;
}


LandUseChangeBenchmarkModel::ScalarRaster
    LandUseChangeBenchmarkModel::elevation() const
{
    ScalarRaster elevation{_raster_shape, _partition_shape};

    return elevation;
}

}  // namespace lue
