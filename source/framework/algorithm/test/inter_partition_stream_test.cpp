#define BOOST_TEST_MODULE lue framework algorithm inter_partition_stream
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace {

    template<typename Index, lue::Rank rank>
    using Material = lue::InterPartitionStreamMaterial<Index, rank>;


    template<typename Index, lue::Rank rank>
    using ArrayPartitionIO = lue::ArrayPartitionIO<Index, rank, Material<Index, rank>>;

}  // Anonymous namespace


// Forward declare check_component_is_equal ...
namespace lue::test {

    template<typename Index, Rank rank>
    void check_component_is_equal(
        ::ArrayPartitionIO<Index, rank> const& array_partition_io_we_have,
        ::ArrayPartitionIO<Index, rank> const& array_partition_io_we_want);

}  // namespace lue::test


// ... and only now include compare.hpp which uses check_component_is_equal
#include "lue/framework/test/hpx_unit_test.hpp"
#include "stream.hpp"


namespace lue::test {

    template<typename Index, Rank rank>
    void check_material_is_equal(
        ::Material<Index, rank> const& material_we_have, ::Material<Index, rank> const& material_we_want)
    {
        BOOST_CHECK_EQUAL(material_we_have.stream_class(), material_we_want.stream_class());
        BOOST_CHECK_EQUAL(material_we_have.count(), material_we_want.count());

        {
            auto idxs1 = material_we_have.input_cells_idxs();
            auto idxs2 = material_we_want.input_cells_idxs();

            std::sort(idxs1.begin(), idxs1.end());
            std::sort(idxs2.begin(), idxs2.end());

            BOOST_CHECK_EQUAL_COLLECTIONS(idxs1.begin(), idxs1.end(), idxs2.begin(), idxs2.end());
        }
    }


    template<typename Index, Rank rank>
    void check_component_is_equal(
        ::ArrayPartitionIO<Index, rank> const& array_partition_io_we_have,
        ::ArrayPartitionIO<Index, rank> const& array_partition_io_we_want)
    {
        using ArrayPartitionIO = ::ArrayPartitionIO<Index, rank>;

        ArrayPartitionIO array_partition_io_we_have_local{
            hpx::components::migrate(array_partition_io_we_have, hpx::find_here())};
        array_partition_io_we_have_local.wait();
        BOOST_REQUIRE(array_partition_io_we_have_local.is_ready());
        auto array_partition_io_we_have_ptr{detail::ready_component_ptr(array_partition_io_we_have_local)};
        auto const& array_partition_io_we_have_server{*array_partition_io_we_have_ptr};
        auto const& array_partition_io_we_have_data{array_partition_io_we_have_server.data()};

        ArrayPartitionIO array_partition_io_we_want_local{
            hpx::components::migrate(array_partition_io_we_want, hpx::find_here())};
        array_partition_io_we_want_local.wait();
        BOOST_REQUIRE(array_partition_io_we_want_local.is_ready());
        auto array_partition_io_we_want_ptr{detail::ready_component_ptr(array_partition_io_we_want_local)};
        auto const& array_partition_io_we_want_server{*array_partition_io_we_want_ptr};
        auto const& array_partition_io_we_want_data{array_partition_io_we_want_server.data()};

        BOOST_CHECK_EQUAL(
            array_partition_io_we_have_data.partition_shape(),
            array_partition_io_we_want_data.partition_shape());

        {
            auto idxs1 = array_partition_io_we_have_data.input_cells_idxs();
            auto idxs2 = array_partition_io_we_want_data.input_cells_idxs();

            BOOST_CHECK_EQUAL_COLLECTIONS(idxs1.begin(), idxs1.end(), idxs2.begin(), idxs2.end());
        }

        {
            auto offsets1 = array_partition_io_we_have_data.offsets();
            auto offsets2 = array_partition_io_we_want_data.offsets();

            BOOST_REQUIRE_EQUAL(offsets1.size(), offsets2.size());

            for (auto it1 = offsets1.begin(), it2 = offsets2.begin(); it1 != offsets1.end(); ++it1, ++it2)
            {
                BOOST_CHECK_EQUAL(it1->first, it2->first);
                BOOST_CHECK_EQUAL(it1->second, it2->second);
            }
        }

        {
            auto values1 = array_partition_io_we_have_data.values();
            auto values2 = array_partition_io_we_want_data.values();

            BOOST_REQUIRE_EQUAL(values1.size(), values2.size());

            for (auto it1 = values1.begin(), it2 = values2.begin(); it1 != values1.end(); ++it1, ++it2)
            {
                BOOST_CHECK_EQUAL(it1->first, it2->first);
                check_material_is_equal(it1->second, it2->second);
            }
        }
    }

}  // namespace lue::test


// // Only now include this header. void check_component_is_equal must have been seen first.
// #include "lue/framework/test/compare.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using CountElement = std::uint64_t;
    using InflowCount = lue::PartitionedArray<CountElement, rank>;

    using MaterialElement = lue::InterPartitionStreamMaterial<lue::Index, rank>;
    using PartitionIO = lue::detail::ArrayPartitionIO<lue::Index, rank, MaterialElement>;
    using PartitionIOComponent = lue::ArrayPartitionIO<lue::Index, rank, MaterialElement>;
    using PartitionIOComponents = lue::ComponentArray<PartitionIOComponent, rank>;
    using Offset = typename PartitionIO::Offset;

    using StreamClass = lue::StreamClass;
    using StreamCells = lue::PartitionedArray<StreamClass, rank>;
    StreamClass const t{lue::inter_partition_stream_cell};
    StreamClass const f{lue::intra_partition_stream_cell};

    template<
        typename Policies,
        typename MaterialElement,
        typename FlowDirectionArray,
        typename StreamCells,
        typename PartitionIOComponents>
    void test_inter_partition_stream(
        Policies const& policies,
        FlowDirectionArray const& flow_direction,
        StreamCells const& stream_cells_we_want,
        PartitionIOComponents const& partition_io_we_want)
    {
        auto const [stream_cells_we_got, partition_io_we_got] =
            lue::inter_partition_stream<Policies>(policies, flow_direction);

        // Verify all cells marked as part of the inter-partition stream match
        lue::test::check_arrays_are_equal(stream_cells_we_got, stream_cells_we_want);

        // Verify the input/output cells found match, and the number of
        // cells contributing to each output cell
        lue::test::check_arrays_are_equal(partition_io_we_got, partition_io_we_want);
    }


    template<
        typename MaterialElement,
        typename FlowDirectionArray,
        typename StreamCells,
        typename PartitionIOComponents>
    void test_inter_partition_stream(
        FlowDirectionArray const& flow_direction,
        StreamCells const& stream_cells_we_want,
        PartitionIOComponents const& partition_io_we_want)
    {
        using FlowDirectionElement = lue::ElementT<FlowDirectionArray>;
        using Policies = lue::policy::inter_partition_stream::DefaultPolicies<FlowDirectionElement>;

        test_inter_partition_stream<Policies, MaterialElement>(
            Policies{}, flow_direction, stream_cells_we_want, partition_io_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(parallel_east)
{
    Offset const offset{0, 1};

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io1{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io1.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io1.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io1.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    PartitionIO io2{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io2.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io2.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io2.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    PartitionIO io3{partition_shape, {}};
    io3.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io3.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 3});
    io3.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io4{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io4.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io4.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io4.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    PartitionIO io5{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io5.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io5.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io5.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    PartitionIO io6{partition_shape, {}};
    io6.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io7{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io7.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io7.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io7.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    PartitionIO io8{partition_shape, {{0, 0}, {1, 0}, {2, 0}}};
    io8.add_output_cell({0, 2}, offset, MaterialElement{{{0, 0}}, t, 3});
    io8.add_output_cell({1, 2}, offset, MaterialElement{{{1, 0}}, t, 3});
    io8.add_output_cell({2, 2}, offset, MaterialElement{{{2, 0}}, t, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    Offset const offset{1, 1};

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io0.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io0.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});
    io0.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io0.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io1{partition_shape, {{1, 0}, {2, 0}}};
    io1.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io1.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io1.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io1.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io1.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io2{partition_shape, {{1, 0}, {2, 0}}};
    io2.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io2.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io2.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io2.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io2.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io3{partition_shape, {{0, 1}, {0, 2}}};
    io3.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io3.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io3.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});
    io3.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io3.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io4{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0}}};
    io4.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io4.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io4.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io4.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io4.add_output_cell({2, 2}, offset, MaterialElement{{{0, 0}}, t, 3});

    PartitionIO io5{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0}}};
    io5.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io5.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io5.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io5.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io5.add_output_cell({2, 2}, offset, MaterialElement{{{0, 0}}, t, 3});

    PartitionIO io6{partition_shape, {{0, 1}, {0, 2}}};
    io6.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io6.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io6.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});
    io6.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io6.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io7{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0}}};
    io7.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io7.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io7.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io7.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io7.add_output_cell({2, 2}, offset, MaterialElement{{{0, 0}}, t, 3});

    PartitionIO io8{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0}}};
    io8.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io8.add_output_cell({1, 2}, offset, MaterialElement{{{0, 1}}, t, 2});
    io8.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});
    io8.add_output_cell({2, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io8.add_output_cell({2, 2}, offset, MaterialElement{{{0, 0}}, t, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
            {
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
                se,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                t,
                f,
                f,
                t,
                t,
                f,
            },
            {
                f,
                f,
                f,
                t,
                f,
                f,
                t,
                t,
                f,
            },
            {
                f,
                t,
                t,
                f,
                f,
                t,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                t,
                t,
                f,
                f,
                t,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    Offset const offset{1, 0};

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io1{partition_shape, {}};
    io1.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io1.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 3});
    io1.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io2{partition_shape, {}};
    io2.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io3{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io3.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io3.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io3.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    PartitionIO io4{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io4.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io4.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io4.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    PartitionIO io5{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io5.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io5.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io5.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    PartitionIO io6{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io6.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io6.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io6.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    PartitionIO io7{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io7.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io7.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io7.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    PartitionIO io8{partition_shape, {{0, 0}, {0, 1}, {0, 2}}};
    io8.add_output_cell({2, 0}, offset, MaterialElement{{{0, 0}}, t, 3});
    io8.add_output_cell({2, 1}, offset, MaterialElement{{{0, 1}}, t, 3});
    io8.add_output_cell({2, 2}, offset, MaterialElement{{{0, 2}}, t, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
            {
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
                s,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    Offset const offset{1, -1};

    PartitionIO io0{partition_shape, {{1, 2}, {2, 2}}};
    io0.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io0.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io0.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io0.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io1{partition_shape, {{1, 2}, {2, 2}}};
    io1.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io1.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io1.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io1.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io1.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io2{partition_shape, {}};
    io2.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io2.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io2.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io2.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    PartitionIO io3{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}}};
    io3.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io3.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io3.add_output_cell({2, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io3.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io3.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io4{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}}};
    io4.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io4.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io4.add_output_cell({2, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io4.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io4.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io5{partition_shape, {{0, 0}, {0, 1}}};
    io5.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io5.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io5.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io5.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io5.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    PartitionIO io6{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}}};
    io6.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io6.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io6.add_output_cell({2, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io6.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io6.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io7{partition_shape, {{0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}}};
    io7.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io7.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io7.add_output_cell({2, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io7.add_output_cell({2, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io7.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io8{partition_shape, {{0, 0}, {0, 1}}};
    io8.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io8.add_output_cell({1, 0}, offset, MaterialElement{{{0, 1}}, t, 2});
    io8.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({2, 1}, offset, MaterialElement{{}, f, 2});
    io8.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
            {
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
                sw,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                f,
                f,
                f,
                f,
                f,
                t,
                f,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                t,
                f,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                f,
                t,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                f,
                t,
                f,
                f,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    Offset const offset{0, -1};

    PartitionIO io0{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io0.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io0.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io0.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io1{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io1.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io1.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io1.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io2{partition_shape, {}};
    io2.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});

    PartitionIO io3{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io3.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io3.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io3.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io4{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io4.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io4.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io4.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io5{partition_shape, {}};
    io5.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io5.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 3});
    io5.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});

    PartitionIO io6{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io6.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io6.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io6.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io7{partition_shape, {{0, 2}, {1, 2}, {2, 2}}};
    io7.add_output_cell({0, 0}, offset, MaterialElement{{{0, 2}}, t, 3});
    io7.add_output_cell({1, 0}, offset, MaterialElement{{{1, 2}}, t, 3});
    io7.add_output_cell({2, 0}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io8{partition_shape, {}};
    io8.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
            {
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
                w,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    Offset const offset{-1, -1};

    PartitionIO io0{partition_shape, {{0, 2}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}};
    io0.add_output_cell({0, 0}, offset, MaterialElement{{{2, 2}}, t, 3});
    io0.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io0.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io0.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io0.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io1{partition_shape, {{0, 2}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}};
    io1.add_output_cell({0, 0}, offset, MaterialElement{{{2, 2}}, t, 3});
    io1.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io1.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io1.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io1.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io2{partition_shape, {{2, 0}, {2, 1}}};
    io2.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io2.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io2.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io2.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io2.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io3{partition_shape, {{0, 2}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}};
    io3.add_output_cell({0, 0}, offset, MaterialElement{{{2, 2}}, t, 3});
    io3.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io3.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io3.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io3.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io4{partition_shape, {{0, 2}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}};
    io4.add_output_cell({0, 0}, offset, MaterialElement{{{2, 2}}, t, 3});
    io4.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io4.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io4.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io4.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io5{partition_shape, {{2, 0}, {2, 1}}};
    io5.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io5.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io5.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io5.add_output_cell({1, 0}, offset, MaterialElement{{{2, 1}}, t, 2});
    io5.add_output_cell({2, 0}, offset, MaterialElement{{{2, 0}}, t, 1});

    PartitionIO io6{partition_shape, {{0, 2}, {1, 2}}};
    io6.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io6.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io6.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io6.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});

    PartitionIO io7{partition_shape, {{0, 2}, {1, 2}}};
    io7.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io7.add_output_cell({0, 1}, offset, MaterialElement{{{1, 2}}, t, 2});
    io7.add_output_cell({0, 2}, offset, MaterialElement{{{0, 2}}, t, 1});
    io7.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io7.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});

    PartitionIO io8{partition_shape, {}};
    io8.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io8.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 1});
    io8.add_output_cell({1, 0}, offset, MaterialElement{{}, f, 2});
    io8.add_output_cell({2, 0}, offset, MaterialElement{{}, f, 1});


    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
            {
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
                nw,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                t,
                f,
                f,
                t,
                t,
                f,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                t,
                f,
                f,
                t,
                t,
                f,
            },
            {
                f,
                t,
                t,
                f,
                f,
                t,
                f,
                f,
                f,
            },
            {
                f,
                t,
                t,
                f,
                f,
                t,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    Offset const offset{-1, 0};

    PartitionIO io0{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io0.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io0.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io0.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io1{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io1.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io1.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io1.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io2{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io2.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io2.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io2.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io3{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io3.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io3.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io3.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io4{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io4.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io4.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io4.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io5{partition_shape, {{2, 0}, {2, 1}, {2, 2}}};
    io5.add_output_cell({0, 0}, offset, MaterialElement{{{2, 0}}, t, 3});
    io5.add_output_cell({0, 1}, offset, MaterialElement{{{2, 1}}, t, 3});
    io5.add_output_cell({0, 2}, offset, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io6{partition_shape, {}};
    io6.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io7{partition_shape, {}};
    io7.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io7.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 3});
    io7.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});

    PartitionIO io8{partition_shape, {}};
    io8.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
            {
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
                n,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    Offset const offset{-1, 1};

    PartitionIO io0{partition_shape, {{2, 1}, {2, 2}}};
    io0.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io0.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io0.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io0.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io0.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io1{partition_shape, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
    io1.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io1.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io1.add_output_cell({0, 2}, offset, MaterialElement{{{2, 0}}, t, 3});
    io1.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io1.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io2{partition_shape, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
    io2.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io2.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io2.add_output_cell({0, 2}, offset, MaterialElement{{{2, 0}}, t, 3});
    io2.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io2.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io3{partition_shape, {{2, 1}, {2, 2}}};
    io3.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io3.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io3.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io3.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io3.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io4{partition_shape, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
    io4.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io4.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io4.add_output_cell({0, 2}, offset, MaterialElement{{{2, 0}}, t, 3});
    io4.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io4.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io5{partition_shape, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
    io5.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io5.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io5.add_output_cell({0, 2}, offset, MaterialElement{{{2, 0}}, t, 3});
    io5.add_output_cell({1, 2}, offset, MaterialElement{{{2, 1}}, t, 2});
    io5.add_output_cell({2, 2}, offset, MaterialElement{{{2, 2}}, t, 1});

    PartitionIO io6{partition_shape, {}};
    io6.add_output_cell({0, 0}, offset, MaterialElement{{}, f, 1});
    io6.add_output_cell({0, 1}, offset, MaterialElement{{}, f, 2});
    io6.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io6.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io6.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    PartitionIO io7{partition_shape, {{0, 0}, {1, 0}}};
    io7.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io7.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io7.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io7.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io7.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    PartitionIO io8{partition_shape, {{0, 0}, {1, 0}}};
    io8.add_output_cell({0, 0}, offset, MaterialElement{{{0, 0}}, t, 1});
    io8.add_output_cell({0, 1}, offset, MaterialElement{{{1, 0}}, t, 2});
    io8.add_output_cell({0, 2}, offset, MaterialElement{{}, f, 3});
    io8.add_output_cell({1, 2}, offset, MaterialElement{{}, f, 2});
    io8.add_output_cell({2, 2}, offset, MaterialElement{{}, f, 1});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
            {
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
                ne,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                f,
                f,
                f,
                f,
                f,
                t,
                f,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                t,
                f,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                f,
                t,
                f,
                f,
                f,
                f,
                f,
            },
            {
                t,
                t,
                f,
                t,
                f,
                f,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(pit)
{
    // +---+---+---+
    // | 🡦 | 🡣 | 🡧 |
    // +---+---+---+
    // | 🡢 | ■ | 🡠 |
    // +---+---+---+
    // | 🡥 | 🡡 | 🡤 |
    // +---+---+---+

    PartitionIO io0{partition_shape, {}};
    // No partition output cells...

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        partition_shape,
        partition_shape,
        {{
            se,
            s,
            sw,
            e,
            p,
            w,
            ne,
            n,
            nw,
        }});

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        partition_shape,
        partition_shape,
        {{
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
        }});

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(diverge)
{
    // +---+---+---+
    // | 🡤 | 🡡 | 🡥 |
    // +---+---+---+
    // | 🡠 | 🡣 | 🡢 |
    // +---+---+---+
    // | 🡧 | 🡣 | 🡦 |
    // +---+---+---+

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({0, 0}, Offset{-1, -1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({0, 1}, Offset{-1, 0}, MaterialElement{{}, f, 1});
    io0.add_output_cell({0, 2}, Offset{-1, 1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({1, 0}, Offset{0, -1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({1, 2}, Offset{0, 1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({2, 0}, Offset{1, -1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({2, 1}, Offset{1, 0}, MaterialElement{{}, f, 2});
    io0.add_output_cell({2, 2}, Offset{1, 1}, MaterialElement{{}, f, 1});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        partition_shape,
        partition_shape,
        {{
            nw,
            n,
            ne,
            w,
            s,
            e,
            sw,
            s,
            se,
        }});

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        partition_shape,
        partition_shape,
        {{
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
        }});

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(converge)
{
    // +---+---+---+
    // | 🡦 | 🡣 | 🡧 |
    // +---+---+---+
    // | 🡢 | 🡣 | 🡠 |
    // +---+---+---+
    // | 🡢 | 🡣 | 🡠 |
    // +---+---+---+

    PartitionIO io0{partition_shape, {}};
    // All cells contribute
    io0.add_output_cell({2, 1}, Offset{1, 0}, MaterialElement{{}, f, 9});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        partition_shape,
        partition_shape,
        {{
            se,
            s,
            sw,
            e,
            s,
            w,
            e,
            s,
            w,
        }});

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        partition_shape,
        partition_shape,
        {{
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
            f,
        }});

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
        });

    test_inter_partition_stream<MaterialElement>(flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(all_no_data_case)
{
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+

    using DomainPolicy = lue::policy::AllValuesWithinDomain<FlowDirectionElement>;

    // Outputs
    using StreamPolicies = lue::policy::OutputPolicies<
        lue::policy::MarkNoDataByValue<StreamClass>,
        lue::policy::AllValuesWithinRange<StreamClass, FlowDirectionElement>>;

    // Inputs
    using FlowDirectionPolicies = lue::policy::SpatialOperationInputPolicies<
        lue::policy::DetectNoDataByValue<FlowDirectionElement>,
        lue::policy::FlowDirectionHalo<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        DomainPolicy,
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;


    StreamPolicies stream_policies{lue::policy::MarkNoDataByValue<StreamClass>{99}};
    FlowDirectionPolicies flow_direction_policies{lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{DomainPolicy{}, stream_policies, flow_direction_policies};

    PartitionIO io0{partition_shape, {}};

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        partition_shape,
        partition_shape,
        {{
            nd,
            nd,
            nd,
            nd,
            nd,
            nd,
            nd,
            nd,
            nd,
        }});

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        partition_shape,
        partition_shape,
        {{
            99,
            99,
            99,
            99,
            99,
            99,
            99,
            99,
            99,
        }});

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
        });

    test_inter_partition_stream<Policies, MaterialElement>(
        policies, flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(no_data_case)
{
    using DomainPolicy = lue::policy::AllValuesWithinDomain<FlowDirectionElement>;

    // Outputs
    using StreamPolicies = lue::policy::OutputPolicies<
        lue::policy::MarkNoDataByValue<StreamClass>,
        lue::policy::AllValuesWithinRange<StreamClass, FlowDirectionElement>>;

    // Inputs
    using FlowDirectionPolicies = lue::policy::SpatialOperationInputPolicies<
        lue::policy::DetectNoDataByValue<FlowDirectionElement>,
        lue::policy::FlowDirectionHalo<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        DomainPolicy,
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    StreamPolicies stream_policies{lue::policy::MarkNoDataByValue<StreamClass>{99}};
    FlowDirectionPolicies flow_direction_policies{lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{DomainPolicy{}, stream_policies, flow_direction_policies};

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({1, 2}, Offset{0, 1}, MaterialElement{{}, f, 1});
    io0.add_output_cell({2, 1}, Offset{1, 0}, MaterialElement{{}, f, 6});

    PartitionIO io1{partition_shape, {{1, 0}}};
    io1.add_output_cell({0, 2}, Offset{0, 1}, MaterialElement{{}, f, 3});
    io1.add_output_cell({1, 2}, Offset{0, 1}, MaterialElement{{{1, 0}}, t, 3});
    io1.add_output_cell({2, 2}, Offset{0, 1}, MaterialElement{{}, f, 3});

    auto flow_direction_array = lue::test::create_partitioned_array<FlowDirectionArray>(
        {3, 6},
        partition_shape,
        {
            {
                e,
                s,
                nd,
                e,
                s,
                e,
                e,
                s,
                nd,
            },
            {
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
                e,
            },
        });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        {3, 6},
        partition_shape,
        {
            {
                f,
                f,
                99,
                f,
                f,
                f,
                f,
                f,
                99,
            },
            {
                f,
                f,
                f,
                t,
                t,
                t,
                f,
                f,
                f,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
        });

    test_inter_partition_stream<Policies, MaterialElement>(
        policies, flow_direction_array, stream_cells_array, io_components);
}


BOOST_AUTO_TEST_CASE(merging_streams_case)
{
    using DomainPolicy = lue::policy::AllValuesWithinDomain<FlowDirectionElement>;

    // Outputs
    using StreamPolicies = lue::policy::OutputPolicies<
        lue::policy::MarkNoDataByValue<StreamClass>,
        lue::policy::AllValuesWithinRange<StreamClass, FlowDirectionElement>>;

    // Inputs
    using FlowDirectionPolicies = lue::policy::SpatialOperationInputPolicies<
        lue::policy::DetectNoDataByValue<FlowDirectionElement>,
        lue::policy::FlowDirectionHalo<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        DomainPolicy,
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    StreamPolicies stream_policies{lue::policy::MarkNoDataByValue<StreamClass>{99}};
    FlowDirectionPolicies flow_direction_policies{lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{DomainPolicy{}, stream_policies, flow_direction_policies};

    PartitionIO io0{partition_shape, {}};
    io0.add_output_cell({2, 2}, Offset{1, 1}, MaterialElement{{}, f, 1});

    PartitionIO io1{partition_shape, {}};
    io1.add_output_cell({0, 1}, Offset{-1, -1}, MaterialElement{{}, f, 1});
    io1.add_output_cell({0, 2}, Offset{1, 1}, MaterialElement{{}, f, 1});
    io1.add_output_cell({1, 2}, Offset{0, 1}, MaterialElement{{}, f, 1});
    io1.add_output_cell({2, 1}, Offset{1, 0}, MaterialElement{{}, f, 2});

    PartitionIO io2{partition_shape, {{1, 0}, {1, 0}}};
    io2.add_output_cell({2, 1}, Offset{1, 0}, MaterialElement{{{1, 0}, {1, 0}}, t, 6});
    io2.add_output_cell({2, 2}, Offset{1, -1}, MaterialElement{{}, f, 1});

    PartitionIO io3{partition_shape, {}};
    io3.add_output_cell({0, 2}, Offset{0, 1}, MaterialElement{{}, f, 1});
    io3.add_output_cell({1, 2}, Offset{-1, 1}, MaterialElement{{}, f, 1});
    io3.add_output_cell({2, 2}, Offset{0, 1}, MaterialElement{{}, f, 1});

    PartitionIO io4{
        partition_shape, {{0, 0}, {0, 0}, {0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 0}, {2, 2}, {2, 2}}};
    io4.add_output_cell(
        {2, 1},
        Offset{1, 0},
        MaterialElement{{{0, 0}, {0, 0}, {0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 0}, {2, 2}, {2, 2}}, t, 8});

    PartitionIO io5{partition_shape, {{0, 1}, {0, 1}}};
    io5.add_output_cell({0, 0}, Offset{0, -1}, MaterialElement{{{0, 1}, {0, 1}}, t, 2});
    io5.add_output_cell({1, 0}, Offset{0, -1}, MaterialElement{{}, f, 1});
    io5.add_output_cell({2, 0}, Offset{0, -1}, MaterialElement{{}, f, 1});

    PartitionIO io6{partition_shape, {{2, 2}, {2, 2}}};
    io6.add_output_cell({2, 2}, Offset{1, 0}, MaterialElement{{{2, 2}, {2, 2}}, t, 1});

    PartitionIO io7{partition_shape, {{0, 1}, {1, 2}, {2, 2}}};
    io7.add_output_cell({1, 0}, Offset{1, -1}, MaterialElement{{{0, 1}, {1, 2}}, t, 4});
    io7.add_output_cell({2, 0}, Offset{0, -1}, MaterialElement{{{2, 2}}, t, 3});

    PartitionIO io8{partition_shape, {}};
    io8.add_output_cell({0, 0}, Offset{-1, -1}, MaterialElement{{}, f, 1});
    io8.add_output_cell({1, 0}, Offset{0, -1}, MaterialElement{{}, f, 1});
    io8.add_output_cell({2, 0}, Offset{0, -1}, MaterialElement{{}, f, 1});

    auto flow_direction_array = lue::test::merging_streams();
    //     lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
    //         {
    //             { // 0, 0
    //                 nd, nd, nd,
    //                 nd, nd, nd,
    //                 nd, nd, se,
    //             },
    //             { // 0, 1
    //                 nd, nw, se,
    //                 nd,  s,  e,
    //                 nd,  s, nd,
    //             },
    //             { // 0, 2
    //                 nd,  s, sw,
    //                 se,  s,  w,
    //                 nd,  s, sw,
    //             },
    //             { // 1, 0
    //                 nd, nd,  e,
    //                 nd, nd, ne,
    //                 nd, nd,  e,
    //             },
    //             { // 1, 1
    //                  e,  s,  w,
    //                 nd,  s,  w,
    //                  e,  s,  w,
    //             },
    //             { // 1, 2
    //                  w,  w, nd,
    //                  w, nd, nd,
    //                  w, nd, nd,
    //             },
    //             { // 2, 0
    //                 nd, nd, nd,
    //                 nd, nd, nd,
    //                 nd, nd,  s,
    //             },
    //             { // 2, 1
    //                 nd,  s, nd,
    //                 sw,  w,  w,
    //                  w,  w,  w,
    //             },
    //             { // 2, 2
    //                 nw, nd, nd,
    //                  w, nd, nd,
    //                  w, nd, nd,
    //             },
    //         });

    auto stream_cells_array = lue::test::create_partitioned_array<StreamCells>(
        array_shape,
        partition_shape,
        {
            {
                99,
                99,
                99,
                99,
                99,
                99,
                99,
                99,
                f,
            },
            {
                99,
                f,
                f,
                99,
                f,
                f,
                99,
                f,
                99,
            },
            {
                99,
                f,
                f,
                t,
                f,
                f,
                99,
                t,
                f,
            },
            {
                99,
                99,
                f,
                99,
                99,
                f,
                99,
                99,
                f,
            },
            {
                t,
                t,
                t,
                99,
                t,
                t,
                t,
                t,
                t,
            },
            {
                t,
                t,
                99,
                f,
                99,
                99,
                f,
                99,
                99,
            },
            {
                99,
                99,
                99,
                99,
                99,
                99,
                99,
                99,
                t,
            },
            {
                99,
                t,
                99,
                t,
                t,
                t,
                t,
                t,
                t,
            },
            {
                f,
                99,
                99,
                f,
                99,
                99,
                f,
                99,
                99,
            },
        });

    auto io_components = lue::test::create_component_array<PartitionIOComponents>(
        flow_direction_array.localities(),
        {
            io0,
            io1,
            io2,
            io3,
            io4,
            io5,
            io6,
            io7,
            io8,
        });

    test_inter_partition_stream<Policies, MaterialElement>(
        policies, flow_direction_array, stream_cells_array, io_components);
}
