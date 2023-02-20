#pragma once
#include "wildfire_model_base.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include <memory>


namespace lue {
    namespace benchmark {
        namespace detail {

            class Model;

        }  // namespace detail


        class WildfireBenchmarkModel: public BenchmarkModel<WildfireModelBase::NominalElement, 2>
        {

            public:

                WildfireBenchmarkModel(
                    Environment const& environment, Task const& task, std::size_t max_tree_depth);

                WildfireBenchmarkModel(WildfireBenchmarkModel const&) = default;

                WildfireBenchmarkModel(WildfireBenchmarkModel&&) = default;

                ~WildfireBenchmarkModel();

                WildfireBenchmarkModel& operator=(WildfireBenchmarkModel const&) = default;

                WildfireBenchmarkModel& operator=(WildfireBenchmarkModel&&) = default;

            private:

                void do_initialize() override;

                void do_simulate(Count time_step) override;

                std::unique_ptr<detail::Model> _model;
        };

    }  // namespace benchmark
}  // namespace lue
