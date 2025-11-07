#define BOOST_TEST_MODULE lue framework algorithm downstream
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/downstream.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(pcraster_example)
{
    using namespace lue::test;

    using MaterialElement = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;
    using Shape = lue::ShapeT<FlowDirectionArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    auto const x{lue::policy::no_data_value<MaterialElement>};

    auto const flow_direction = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                 s, s,  s, sw, sw,
                 s, s, sw, sw, sw,
                se, s, sw,  w, sw,
                se, s, sw,  w,  w,
                 e, p,  w,  w,  w,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const material = lue::test::create_partitioned_array<MaterialArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                1, 1, 2, 2, 4,
                1, 1, 2, 2, 4,
                2, 2, x, 4, 4,
                2, 2, 2, 4, 4,
                3, 7, 5, 5, 5,
            },
            // clang-format on
            // NOLINTEND
        });

    auto const material_we_got = lue::value_policies::downstream(flow_direction, material);
    auto const material_we_want = lue::test::create_partitioned_array<MaterialArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                1, 1, 2, 2, 2,
                2, 2, 2, x, 4,
                2, 2, x, x, 4,
                7, 7, 7, 2, 4,
                7, 7, 7, 5, 5,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(material_we_got, material_we_want);
}


BOOST_AUTO_TEST_CASE(no_data)
{
    using namespace lue::test;

    using MaterialElement = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;
    using Shape = lue::ShapeT<FlowDirectionArray>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    auto const x{lue::policy::no_data_value<MaterialElement>};
    auto const u{lue::policy::no_data_value<FlowDirectionElement>};

    {
        auto const flow_direction = lue::test::create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                // clang-format on
                // NOLINTEND
            });
        auto const material = lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                // clang-format on
                // NOLINTEND
            });
        auto const material_we_got = lue::value_policies::downstream(flow_direction, material);
        auto const material_we_want = lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

        lue::test::check_arrays_are_equal(material_we_got, material_we_want);
    }

    {
        auto const flow_direction = lue::test::create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    u, u, u,
                    u, u, u,
                    u, u, u,
                },
                // clang-format on
                // NOLINTEND
            });
        auto const material = lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    5, 5, 5,
                    5, 5, 5,
                    5, 5, 5,
                },
                // clang-format on
                // NOLINTEND
            });
        auto const material_we_got = lue::value_policies::downstream(flow_direction, material);
        auto const material_we_want = lue::test::create_partitioned_array<MaterialArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

        lue::test::check_arrays_are_equal(material_we_got, material_we_want);
    }
}
