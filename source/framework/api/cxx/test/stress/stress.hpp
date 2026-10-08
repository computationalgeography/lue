#pragma once
#include "lue/framework/model/progressor.hpp"
#include "lue/framework/model/simulate.hpp"
#include "lue/utility/application.hpp"
#include <hpx/iostream.hpp>


namespace lue::stress {

    namespace {

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


        using SilentProgressor = lue::Progressor;

    }  // Anonymous namespace


    template<typename Model>
    class StressTest: public lue::utility::Application
    {

        public:

            StressTest(int argc, char** argv):

                lue::utility::Application{
                    [&]() -> cxxopts::Options
                    {
                        cxxopts::Options options{
                            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
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

                Model model{
                    {array_dimension_size, array_dimension_size},
                    {partition_dimension_size, partition_dimension_size}};

                // bool const silent = true;
                //
                // auto progressor = silent ? Progressor{} : DefaultProgressor{};

                Progressor progressor{};

                lue::run_deterministic(model, progressor, nr_time_steps, rate_limit);

                return EXIT_SUCCESS;
            }
    };


    template<typename Model>
    auto run_stress_test(int argc, char** argv) -> int
    {
        return StressTest<Model>{argc, argv}.run();
    }

}  // namespace lue::stress
