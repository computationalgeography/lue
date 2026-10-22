#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/accu.hpp"
#include "lue/framework/algorithm/value_policies/cast.hpp"
#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework/algorithm/value_policies/d8_flow_direction.hpp"
#include "lue/framework/algorithm/value_policies/inflow_count.hpp"
#include "lue/framework/algorithm/value_policies/open_simplex_noise.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/model.hpp"
#include "lue/framework.hpp"
#include "lue/utility/application.hpp"
#include <hpx/hpx_main.hpp>
#include <hpx/iostream.hpp>


namespace {

    class MyModel: public lue::Model
    {

        public:

            static lue::Rank const rank{2};

            using Shape = lue::Shape<lue::Count, rank>;

            template<typename Element>
            using Array = lue::PartitionedArray<Element, rank>;

            using FloatElement = lue::FloatingPointElement<0>;

            using BooleanArray = Array<lue::BooleanElement>;
            using FloatArray = Array<FloatElement>;
            using FlowDirectionArray = Array<lue::FlowDirectionElement>;


            MyModel(Shape const array_shape, Shape const partition_shape):
                _array_shape{array_shape},
                _partition_shape{partition_shape}

            {
            }


            void initialize() final
            {
                using namespace lue;
                using namespace lue::value_policies;

                BooleanArray const condition =
                    lue::create_partitioned_array<BooleanElement>(_array_shape, _partition_shape, 1);
                FloatArray const x_coordinates = cast<FloatElement>(cell_index<IndexElement>(condition, 1));
                FloatArray const y_coordinates = cast<FloatElement>(cell_index<IndexElement>(condition, 0));
                FloatArray const elevation = open_simplex_noise(x_coordinates, y_coordinates, 5);

                _flow_direction = d8_flow_direction<FlowDirectionElement>(elevation);
                // _material = uniform<FloatElement>(condition, 1, 100);
            }


            auto simulate([[maybe_unused]] lue::Count const time_step) -> hpx::shared_future<void> final
            {
                using namespace lue;
                using namespace lue::value_policies;

                // hpx::cout << std::format("simulate[{}]\n", time_step) << std::flush;

                // // TODO: memory accumulates!!!
                // _material = accu(_flow_direction, _material);
                // return hpx::when_all(_material.partitions().begin(),
                // _material.partitions().end()).share();

                using SmallCountElement = lue::BooleanElement;  // TODO: Rename (LUE_SMALL_COUNT_ELEMENT?)

                auto const inflow_count =
                    lue::value_policies::inflow_count<SmallCountElement>(_flow_direction);

                // // TODO: assert inflow count's data structs contain an single record
                // hpx::cout << std::format(
                //     "inflow_count_use_count_by: {}\n",
                //     lue::detail::inflow_count_communicator_use_count_by<2>().size()) << std::flush;
                // hpx::cout << std::format(
                //     "inflow_count_communicator_use_finished: {}\n",
                //     lue::detail::inflow_count_communicator_use_finished<2>().size()) << std::flush;

                // hpx::cout << std::format("/simulate[{}]\n", time_step) << std::flush;

                hpx::agas::garbage_collect();

                return hpx::when_all(inflow_count.partitions().begin(), inflow_count.partitions().end())
                    .share();
            }


            // auto simulate([[maybe_unused]] lue::Count const time_step) -> hpx::shared_future<void> final
            // {
            //     using Array = FloatArray;
            //     // using Data = lue::DataT<Array>;
            //
            //     Array meh = lue::create_partitioned_array<FloatElement>(_array_shape, _partition_shape);
            //
            //     return hpx::when_all(meh.partitions().begin(), meh.partitions().end()).share();
            // }


        private:

            Shape _array_shape;

            Shape _partition_shape;

            FlowDirectionArray _flow_direction;

            // FloatArray _material;
    };


    class DefaultProgressor: public lue::Progressor
    {

        public:

            void initialize() final
            {
                hpx::cout << "[" << std::flush;
            }


            void simulate([[maybe_unused]] lue::Count const time_step) final
            {
                hpx::cout << "." << std::flush;
            }


            void finalize() final
            {
                hpx::cout << "]\n" << std::flush;
            }
    };


    class Example: public lue::utility::Application
    {

        public:

            Example(int const argc, char const* const* argv):

                lue::utility::Application{
                    [&]() -> cxxopts::Options
                    {
                        cxxopts::Options options{
                            std::filesystem::path{argv[0]}.filename().string(),
                            "Program to verify memory isn't increasing while iterating"};

                        options.custom_help("");
                        options.positional_help(
                            "<array_dimension_size> <partition_dimension_size> <nr_time_steps> <rate_limit>");
                        options.add_options()
                            // clang-format off
                            ("h,help", "Show usage")
                            ("v,version", "Show version")
                            ("array_dimension_size",
                                "Number of elements along one dimension of the arrays",
                                cxxopts::value<lue::Count>())
                            ("partition_dimension_size",
                                "Number of elements along one dimension of the partitions",
                                cxxopts::value<lue::Count>())
                            ("nr_time_steps",
                                "Number of time steps to iterate",
                                cxxopts::value<lue::Count>())
                            ("rate_limit",
                                "Maximum number of time steps for which to spawn tasks at the same time",
                                cxxopts::value<lue::Count>())
                            // clang-format on
                            ;
                        options.parse_positional(
                            {"array_dimension_size",
                             "partition_dimension_size",
                             "nr_time_steps",
                             "rate_limit"});

                        return options;
                    }(),
                    argc,
                    argv}

            {
            }

        private:

            auto run_implementation() -> int override
            {
                if (!argument_parsed("array_dimension_size") ||
                    !argument_parsed("partition_dimension_size") || !argument_parsed("nr_time_steps") ||
                    !argument_parsed("rate_limit"))
                {
                    throw std::runtime_error("Wrong usage, pass '--help' for more information");
                }

                auto const array_dimension_size = argument<lue::Count>("array_dimension_size");
                auto const partition_dimension_size = argument<lue::Count>("partition_dimension_size");
                auto const nr_time_steps = argument<lue::Count>("nr_time_steps");
                auto const rate_limit = argument<lue::Count>("rate_limit");

                hpx::cout << std::format("nr_time_steps  : {}\n", nr_time_steps) << std::flush;
                hpx::cout << std::format("rate_limit     : {}\n", rate_limit) << std::flush;
                hpx::cout << std::format(
                                 "array_shape    : {} x {}\n", array_dimension_size, array_dimension_size)
                          << std::flush;
                hpx::cout << std::format(
                                 "partition_shape: {} x {}\n",
                                 partition_dimension_size,
                                 partition_dimension_size)
                          << std::flush;

                MyModel model{
                    {array_dimension_size, array_dimension_size},
                    {partition_dimension_size, partition_dimension_size}};
                DefaultProgressor progressor{};

                lue::run_deterministic(model, progressor, nr_time_steps, rate_limit);

                return EXIT_SUCCESS;
            }
    };

}  // Anonymous namespace


auto main(int argc, char** argv) -> int
{
    return Example{argc, argv}.run();
}


// #include "lue/framework/algorithm/scalar.hpp"
//
// #include <hpx/hpx.hpp>
// #include <hpx/hpx_main.hpp>
//
//
// using State = std::vector<double>;
// using StateF = hpx::shared_future<State>;
//
//
// void simulate1(std::size_t const nr_time_steps, std::size_t const nr_elements, int const rate_limit)
// {
//     // No feedback between the time steps: no dependencies between the tasks
//
//     for (size_t time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
//     {
//         auto state_f = hpx::async([nr_elements]() -> auto { return State(nr_elements); });
//
//         if (time_step_idx % rate_limit == 0)
//         {
//             state_f.wait();
//         }
//     }
// }
//
//
// void simulate2(std::size_t const nr_time_steps, std::size_t const nr_elements, int const rate_limit)
// {
//     // Feedback between the time steps: dependencies between the tasks
//
//     StateF state_f = hpx::make_ready_future<State>().share();
//
//     for (size_t time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx)
//     {
//         state_f = state_f.then(
//             [nr_elements]([[maybe_unused]] StateF const& state_f) -> auto { return State(nr_elements); });
//
//         if (time_step_idx % rate_limit == 0)
//         {
//             state_f.wait();
//         }
//     }
// }
//
//
// void simulate3(std::size_t const nr_time_steps, std::size_t const nr_elements, int const rate_limit)
// {
//     // Limit depth of task tree using a sliding semaphore
//
//     std::int64_t const max_difference{rate_limit};
//     auto semaphore = std::make_shared<hpx::sliding_semaphore>(max_difference);
//     StateF current_state_f = hpx::make_ready_future<State>().share();
//     std::int64_t current_time_step = 1;
//
//     for (size_t time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx, ++current_time_step)
//     {
//         StateF next_state_f = current_state_f.then(
//             [nr_elements]([[maybe_unused]] StateF const& state_f) -> auto { return State(nr_elements); });
//
//         if (current_time_step % rate_limit == 0)
//         {
//             next_state_f.then(
//                 [semaphore, current_time_step]([[maybe_unused]] StateF const& state_f) -> void
//                 { semaphore->signal(current_time_step); });
//         }
//
//         semaphore->wait(current_time_step);
//     }
// }
//
//
// void simulate4(std::size_t const nr_time_steps, std::size_t const nr_elements, int const rate_limit)
// {
//     // ...
//
//     // Works:
//     // using Data = std::vector<double>;
//     // using Data = double;
//     // using DataF = hpx::shared_future<Data>;
//     // DataF data_f = hpx::make_ready_future<Data>(nr_elements).share();
//
//     using Scalar = lue::Scalar<std::size_t>;
//     Scalar scalar{nr_elements};
//     using DataF = hpx::shared_future<std::size_t>;
//
//     using State = void;
//     using StateF = hpx::shared_future<State>;
//
//     std::int64_t const max_difference{rate_limit};
//     auto semaphore = std::make_shared<hpx::sliding_semaphore>(max_difference);
//     std::int64_t current_time_step = 1;
//
//     for (size_t time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx, ++current_time_step)
//     {
//         scalar = scalar.future().then(
//             [nr_elements]([[maybe_unused]] DataF const& data_f) -> std::size_t { return nr_elements; });
//
//         StateF state_f = scalar.future();
//
//         if (current_time_step % rate_limit == 0)
//         {
//             state_f.then(
//                 [semaphore, current_time_step]([[maybe_unused]] StateF const& state_f) -> void
//                 { semaphore->signal(current_time_step); });
//         }
//
//         semaphore->wait(current_time_step);
//     }
// }
//
//
// auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int
// {
//     int const rate_limit = 2;
//     std::size_t const nr_time_steps = 1000000000000;
//     std::size_t const nr_elements = 1000000;  // 00000000;
//
//     // Memory does not increase during the simulation
//     // simulate1(nr_time_steps, nr_elements, rate_limit);
//
//     // Memory does not increase during the simulation
//     // simulate2(nr_time_steps, nr_elements, rate_limit);
//
//     // Memory does not increase during the simulation
//     // simulate3(nr_time_steps, nr_elements, rate_limit);
//
//     simulate4(nr_time_steps, nr_elements, rate_limit);
//
//     return 0;
// }


// set(names
//     memory
// )
//
// foreach(name ${names})
//     set(target_name lue_framework_model_test_${name})
//
//     add_executable(${target_name}
//         ${name}_test.cpp
//     )
//
//     target_link_libraries(${target_name}
//         PRIVATE
//             # lue::framework_model
//             # lue::utility
//             HPX::iostreams_component
//             HPX::wrap_main
//     )
// endforeach()
