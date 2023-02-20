#include "polute_air.hpp"
#include "lue/framework/algorithm.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"

// #include "lue/framework/core/debug.hpp"
// #include "lue/framework/core/domain_decomposition.hpp"
// // #include <hpx/config.hpp>
// #include <hpx/hpx.hpp>
// // #include <hpx/lcos/gather.hpp>
// // #include "lue/data_model.hpp"
// // #include <hpx/runtime.hpp>
#include <hpx/iostream.hpp>
#include <fmt/format.h>
// #include <iostream>
// #include <cassert>


namespace fmt {

    template<typename T>
    struct formatter<std::vector<T>>
    {

            template<typename ParseContext>
            constexpr auto parse(ParseContext& ctx)
            {
                return ctx.begin();
            }

            template<typename FormatContext>
            auto format(std::vector<T> const& values, FormatContext& ctx)
            {
                lue_assert(values.size() > 1);

                auto it = ctx.begin();
                it = format_to(it, "[");

                for (auto v_it = values.begin(); v_it != values.end() - 1; ++v_it)
                {
                    it = format_to(it, "{}, ", *v_it);
                }
                it = format_to(it, "{}", values.back());

                it = format_to(it, "]");

                return it;
            }
    };


    template<typename Index, lue::Rank rank>
    struct formatter<lue::Shape<Index, rank>>
    {
            static_assert(rank > 0);

            template<typename ParseContext>
            constexpr auto parse(ParseContext& ctx)
            {
                return ctx.begin();
            }

            template<typename FormatContext>
            auto format(lue::Shape<Index, rank> const& values, FormatContext& ctx)
            {
                auto it = ctx.out();
                it = format_to(it, "[");

                for (auto v_it = values.begin(); v_it != values.end() - 1; ++v_it)
                {
                    it = format_to(it, "{}, ", *v_it);
                }
                it = format_to(it, "{}", values.back());

                it = format_to(it, "]");

                return it;
            }
    };

}  // namespace fmt


namespace {

    // char const* gather_basename()
    // {
    //     // static char const* basename = "/polute_air/gather/";
    //     // static auto const nr_localities = hpx::get_num_localities().get();
    //     // static std::string const basename =
    //     //     fmt::format("/{}/polute_air/gather/", nr_localities);
    //
    //     static std::string const basename = "/polute_air/gather/";
    //
    //     return basename.c_str();
    // }


    // hpx::future<double> max_airpolution(
    //     std::uint64_t const /* nr_time_steps */,
    //     std::uint64_t const /* nr_rows */,
    //     std::uint64_t const /* nr_cols */,
    //     std::uint64_t const /* nr_rows_grain */,
    //     std::uint64_t const /* nr_cols_grain */)
    // {
    //     // TODO Perform some real HPX work
    //
    //     using namespace std::chrono_literals;
    //
    //     // auto const nr_localities = hpx::get_num_localities().get();
    //
    //     // std::this_thread::sleep_for(10s / nr_localities);
    //
    //     std::this_thread::sleep_for(10s);
    //
    //     return hpx::make_ready_future<double>(5.0);
    // }

}  // Anonymous namespace


// HPX_REGISTER_GATHER(double, max_airpolution_gatherer);


namespace lue {

    /*!
        @brief      Calculate average airpolution based on synthetic data
    */
    void polute_air(
        lue::Count const nr_time_steps,
        Shape<lue::Count, 2> const& array_shape,
        Shape<lue::Count, 2> const& partition_shape)
    {
        hpx::cout << fmt::format(
                         "array_shape           : {}\n"
                         "partition_shape       : {}\n",
                         array_shape,
                         partition_shape)
                  << std::flush;

        using Element = double;
        Rank const rank = 2;

        using Array = lue::PartitionedArray<Element, rank>;

        // Create initial array. Partitions will be distributed over the
        // available localities. This currently blocks until the partitions
        // are created and the array is ready.
        Array array{array_shape, partition_shape};

        hpx::cout << fmt::format("shape in partitions   : {}\n", array.partitions().shape()) << std::flush;

        // Fill the initial array. Because of the wait, this blocks until
        // the array is filled.
        hpx::shared_future<Element> fill_value = hpx::make_ready_future<Element>(2);
        lue::fill(array, fill_value).wait();

        hpx::shared_future<Element> parameter1 = hpx::make_ready_future<Element>(1);
        hpx::shared_future<Element> parameter2 = hpx::make_ready_future<Element>(2);
        hpx::shared_future<Element> parameter3 = hpx::make_ready_future<Element>(3);

        for (lue::Index t = 0; t < nr_time_steps; ++t)
        {
            // array = array * parameter1 + array * parameter2 + array * parameter3

            // The arguments used in the calculations are not editted.
            // The results are newly created partitioned arrays with
            // partitions that may not be ready yet.

            auto multiply1 = lue::multiply(array, parameter1);
            auto multiply2 = lue::multiply(array, parameter2);
            auto multiply3 = lue::multiply(array, parameter3);

            auto add1 = lue::add(multiply1, multiply2);
            auto add2 = lue::add(add1, multiply3);

            array = add2;


            // auto multiply1 = lue::multiply(array, parameter1);
            // auto multiply2 = lue::multiply(array, parameter2);
            // auto multiply3 = lue::multiply(array, parameter3);

            // auto multiply4 = lue::multiply(array, parameter1);
            // auto multiply5 = lue::multiply(array, parameter2);
            // auto multiply6 = lue::multiply(array, parameter3);

            // auto multiply7 = lue::multiply(array, parameter1);
            // auto multiply8 = lue::multiply(array, parameter2);

            // auto add1 = lue::add(multiply1, multiply2);
            // auto add2 = lue::add(multiply3, multiply4);
            // auto add3 = lue::add(multiply5, multiply6);
            // auto add4 = lue::add(multiply7, multiply8);

            // auto add5 = lue::add(add1, add2);
            // auto add6 = lue::add(add3, add4);

            // auto add7 = lue::add(add5, add6);

            // // Overwrite the array with a new instance. The partitions stay
            // // alive, since they are input to the above calculations and are
            // // reference counted. They behave like shared futures, which behave
            // // like shared pointers.
            // array = add7;

            hpx::cout << '.' << std::flush;
        }

        hpx::cout << " waiting..." << std::flush;
        hpx::wait_all_n(array.begin(), array.nr_partitions());
        hpx::cout << std::endl;

        {
            Element result = fill_value.get();
            Element parameter = 1 * (parameter1.get() + parameter2.get() + parameter3.get())

                // 2 * (parameter1.get() + parameter2.get() + parameter3.get()) +
                // 1 * (parameter1.get() + parameter2.get())
                ;

            for (lue::Index t = 0; t < nr_time_steps; ++t)
            {
                result = parameter * result;
            }
            hpx::cout << "Result we got : " << array.partitions()[0].data().get()[0] << '\n'
                      << "Result we want: " << result << std::endl;
        }


        // // lue_assert(nr_rows > 0);
        // // lue_assert(nr_cols > 0);
        // // lue_assert(nr_rows_partition < nr_rows);
        // // lue_assert(nr_cols_partition < nr_cols);

        // // using Shape = lue::Shape<std::uint64_t, 2>;

        // // Shape area_shape;
        // // std::copy(array_shape.begin(), array_shape.end(), area_shape.begin());

        // // Shape partition_shape;
        // // std::copy(partition_shape.begin(), array_shape.end(), area_shape.begin());

        // // Shape partition_shape{{nr_rows_partition, nr_cols_partition}};


        // // // // Determine which part of the world we need to handle. We need the
        // // // // location of the grains.
        // // // auto grains = lue::grains(
        // // //     area_shape, grain_shape, hpx::get_num_localities(hpx::launch::sync),
        // // //     hpx::get_locality_id());


        // // hpx::cout << system_description().get();


        // // // For each raster input parameter, we need to setup a spatial
        // // // index with raster partitions. This will make it possible to
        // // // perform spatial queries.

        // //


        // // // auto const locality_id = hpx::get_locality_id();

        // // // // std::cout << "locality: " << locality_id << std::endl;

        // // // // TODO Do something useful on the current locality
        // // // hpx::future<double> local_result = max_airpolution(
        // // //     nr_time_steps, nr_rows, nr_cols, nr_rows_grain, nr_cols_grain);

        // // // // TODO Gather results from all localities
        // // // // When this function is called multiple time (e.g. when
        // // // // benchmarking), subsequent calls to gather must be made unique by
        // // // // passing a count argument
        // // // static int count = 0;

        // // // if(locality_id == 0) {
        // // //     // We are the gather destination site
        // // //     std::vector<double> all_results =
        // // //         hpx::lcos::gather_here(
        // // //             gather_basename(), std::move(local_result),
        // // //             hpx::get_num_localities(hpx::launch::sync), count).get();

        // // //     lue_assert(!all_results.empty());

        // // //     // auto const overall_result =
        // // //     //     *std::max_element(all_results.begin(), all_results.end());

        // // //     // std::cout << fmt::format(
        // // //     //     "Received {} results, of which the maximum is {}\n",
        // // //     //     nr_localities, overall_result);
        // // // }
        // // // else {
        // // //     // We are not the gather source site

        // // //     // Transmit value to gather destination site (locality_id == 0)
        // // //     hpx::lcos::gather_there(
        // // //         gather_basename(), std::move(local_result),
        // // //         count).wait();
        // // // }

        // // // ++count;
    }

}  // namespace lue
