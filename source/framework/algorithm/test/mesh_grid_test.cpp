#define BOOST_TEST_MODULE lue framework algorithm mesh_grid
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/mesh_grid.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


// BOOST_AUTO_TEST_CASE(use_case_1)
// {
//     // 1D mesh grid
//
//     lue::Count const nr_elements{5};
//
//     using Element = double;
//     lue::Rank const rank{1};
//
//     using Array = lue::PartitionedArray<Element, rank>;
//     using Partition = lue::PartitionT<Array>;
//     using PartitionData = lue::DataT<Partition>;
//     using Shape = lue::ShapeT<Array>;
//
//     Shape shape{nr_elements};
//
//     Array input_array{lue::create_partitioned_array<Element>(shape, shape)};
//
//     Element first_value{1};
//     Element step{0.5};
//     auto [result_we_got] = lue::mesh_grid(input_array, first_value, step);
//
//     Array result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
//     lue::wait_all(result_we_want.partitions());
//     result_we_want.partitions()(0)
//         .set_data(PartitionData{
//             shape,
//             std::initializer_list<Element>{
//                 1.0,
//                 1.5,
//                 2.0,
//                 2.5,
//                 3.0,
//             }})
//         .wait();
//
//     lue::test::check_arrays_are_close(result_we_got, result_we_want);
// }


BOOST_AUTO_TEST_CASE(use_case_2)
{
    // 2D mesh grid

    lue::Count const nr_rows{3};
    lue::Count const nr_cols{5};

    using Element = double;
    lue::Rank const rank{2};

    using Array = lue::PartitionedArray<Element, rank>;
    using Partition = lue::PartitionT<Array>;
    using PartitionData = lue::DataT<Partition>;
    using Shape = lue::ShapeT<Array>;

    Shape shape{nr_rows, nr_cols};

    Array input_array{lue::create_partitioned_array<Element>(shape, shape)};

    Element first_value{1};
    Element step{0.5};
    auto [result_we_got1, result_we_got2] = lue::mesh_grid(input_array, first_value, step);

    Array result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    lue::wait_all(result_we_want.partitions());
    result_we_want.partitions()(0, 0)
        .set_data(PartitionData{
            shape,
            std::initializer_list<Element>{
                1.0,
                1.0,
                1.0,
                1.0,
                1.0,
                1.5,
                1.5,
                1.5,
                1.5,
                1.5,
                2.0,
                2.0,
                2.0,
                2.0,
                2.0,
            }})
        .wait();

    lue::test::check_arrays_are_close(result_we_got1, result_we_want);

    result_we_want.partitions()(0, 0)
        .set_data(PartitionData{
            shape,
            std::initializer_list<Element>{
                1.0,
                1.5,
                2.0,
                2.5,
                3.0,
                1.0,
                1.5,
                2.0,
                2.5,
                3.0,
                1.0,
                1.5,
                2.0,
                2.5,
                3.0,
            }})
        .wait();

    lue::test::check_arrays_are_close(result_we_got2, result_we_want);
}
