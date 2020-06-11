#include "wildfire_benchmark_model.hpp"
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/algorithm/uniform.hpp"


namespace lue {

WildfireBenchmarkModel::WildfireBenchmarkModel(
    benchmark::Environment const& /* environment */,
    benchmark::Task const& task,
    std::size_t const max_tree_depth):

    WildfireModelBase{},
    benchmark::BenchmarkModel<WildfireModelBase::NominalElement, 2>{task, max_tree_depth},
    _raster_shape{},
    _partition_shape{},
    _clone{}

{
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        _raster_shape.begin());

    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        _partition_shape.begin());

    _clone = BooleanRaster{_raster_shape, _partition_shape};
}


WildfireBenchmarkModel::BooleanRaster
    WildfireBenchmarkModel::initial_fire() const
{
    return uniform(_clone, 0.0, 1.0) < 1e-5;
}


void WildfireBenchmarkModel::initialize()
{
    Base::initialize();
}


void WildfireBenchmarkModel::simulate(
    Count const time_step)
{
    Base::simulate(time_step);
}


void WildfireBenchmarkModel::do_preprocess()
{
    // TODO
}


void WildfireBenchmarkModel::do_initialize()
{
    // TODO
}


void WildfireBenchmarkModel::do_simulate(
    Count const /* time_step */)
{
    // TODO
}


void WildfireBenchmarkModel::do_postprocess()
{
    // TODO
}

}  // namespace lue
