#pragma once
#include "wildfire_model_base.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {

class WildfireBenchmarkModel:
    private WildfireModelBase,
    public benchmark::BenchmarkModel<WildfireModelBase::NominalElement, 2>
{

public:

    using Base = benchmark::BenchmarkModel<WildfireModelBase::NominalElement, 2>;

                   WildfireBenchmarkModel(
                                        benchmark::Environment const& environment,
                                        benchmark::Task const& task,
                                        std::size_t max_tree_depth);

                   WildfireBenchmarkModel(
                                        WildfireBenchmarkModel const&)=default;

                   WildfireBenchmarkModel(
                                        WildfireBenchmarkModel&&)=default;

                   ~WildfireBenchmarkModel()=default;

    WildfireBenchmarkModel&
                   operator=           (WildfireBenchmarkModel const&)=default;

    WildfireBenchmarkModel&
                   operator=           (WildfireBenchmarkModel&&)=default;

    void           initialize          () override;

    void           simulate            (Count time_step) override;

protected:

private:

    void           do_preprocess       () override;

    void           do_initialize       () override;

    void           do_simulate         (Count time_step) override;

    void           do_postprocess      () override;

    BooleanRaster   initial_fire       () const final;

    RasterShape    _raster_shape;

    RasterShape    _partition_shape;

    BooleanRaster  _clone;

};

}  // namespace lue
