#pragma once
// #include "routing_model_base.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/data_model.hpp"
// #include <memory>


namespace lue::benchmark {
    // namespace detail {

    //     class Model;

    // }  // namespace detail


    class RoutingBenchmarkModel:
        public BenchmarkModel<float, 2>
    {

        public:

            RoutingBenchmarkModel(
                Environment const& environment,
                Task const& task,
                std::size_t max_tree_depth,
                std::string const& array_pathname,
                std::vector<Index> center_cell);

            RoutingBenchmarkModel(RoutingBenchmarkModel const&)=default;

            RoutingBenchmarkModel(RoutingBenchmarkModel&&)=default;

            ~RoutingBenchmarkModel();

            RoutingBenchmarkModel& operator=(RoutingBenchmarkModel const&)=default;

            RoutingBenchmarkModel& operator=(RoutingBenchmarkModel&&)=default;


        private:

            using FlowDirectionElement = std::uint8_t;

            using ScalarElement = float;

            void do_preprocess() override;

            void do_initialize() override;

            void do_simulate(Count time_step) override;

            void do_terminate() override;

            // std::unique_ptr<detail::Model> _model;

            std::string _array_pathname;

            data_model::ID _object_id;

            hdf5::Hyperslab _hyperslab;

            PartitionedArray<FlowDirectionElement, 2> flow_direction;

            PartitionedArray<ScalarElement, 2> interception_rate;

            PartitionedArray<ScalarElement, 2> interception_storage;

            PartitionedArray<ScalarElement, 2> infiltration_rate;

            // PartitionedArray<ScalarElement, 2> infiltration_storage;

    };

}  // namespace lue::benchmark
