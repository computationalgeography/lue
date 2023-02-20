#include "describe_flow_direction.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include "lue/framework/algorithm/policy.hpp"
// #include "lue/framework/core/define.hpp"
// #include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/io.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <iostream>


namespace {

    std::string const usage = fmt::format(
        R"(
Describe flow direction data

usage:
    {0} <input> <phenomenon> <property-set> <elevation-layer> <flow-direction-layer> <partition-shape> <output>
    {0} (-h | --help) | --version

options:
    -h --help             Show this screen
    --version             Show version

See '{0} <command> --help' for more information on a command.
)",
        "lue_describe_flow_direction");

}  // Anonymous namespace


namespace lue {
    namespace utility {

        DescribeFlowDirection::DescribeFlowDirection(std::vector<std::string> const& arguments):

            Application(usage, arguments)

        {
        }


        int DescribeFlowDirection::run_implementation()
        {
            H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

            auto const input_dataset_pathname = argument<std::string>("<input>");
            auto const phenomenon_name = argument<std::string>("<phenomenon>");
            auto const property_set_name = argument<std::string>("<property-set>");
            auto const elevation_layer_name = argument<std::string>("<elevation-layer>");
            auto const flow_direction_layer_name = argument<std::string>("<flow-direction-layer>");
            auto const partition_shape_string = argument<std::string>("<partition-shape>");
            auto const output_dataset_pathname = argument<std::string>("<output>");

            namespace lh5 = lue::hdf5;
            namespace ldm = lue::data_model;

            using DatasetPtr = std::shared_ptr<ldm::Dataset>;
            using RasterView = ldm::constant::RasterView<DatasetPtr>;
            using SpaceBox = typename RasterView::SpaceBox;

            SpaceBox space_box{};
            lh5::Shape grid_shape{};
            ldm::ID object_id{};

            // -------------------------------------------------------------------------
            // Determine raster shape

            // Open and query dataset. Close it again before the actual read
            // is performed!
            {
                auto input_dataset_ptr =
                    std::make_shared<ldm::Dataset>(ldm::open_dataset(input_dataset_pathname, H5F_ACC_RDONLY));

                // Open a view on the raster
                RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

                // Open raster layer and determine its shape
                auto const flow_direction_layer{input_raster_view.layer(flow_direction_layer_name)};

                space_box = input_raster_view.space_box();
                grid_shape = lh5::Shape{input_raster_view.grid_shape()};
                object_id = input_raster_view.object_id();
            }

            // Create partitioned array for reading the flow direction into
            lue::Rank const rank = 2;

            // TODO Read element types from input rasters
            // type_id(array_pathname)
            using ElevationElement = float;
            using FlowDirectionElement = std::uint8_t;

            using ElevationArray = PartitionedArray<ElevationElement, rank>;
            using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;

            using Shape = ShapeT<FlowDirectionArray>;
            Shape const partition_shape = [](std::string const& partition_shape_string)
            {
                std::vector<std::string> extents;
                boost::split(extents, partition_shape_string, [](char c) { return c == 'x'; });
                if (extents.size() != 2)
                {
                    throw std::runtime_error("Partition shape must be formatted as <nr>x<nc>");
                }
                return Shape{std::stoi(extents[0]), std::stoi(extents[1])};
            }(partition_shape_string);

            std::string array_pathname;

            // Read layer into partitioned array, updating the existing partitions
            array_pathname = fmt::format(
                "{}/{}/{}/{}",
                input_dataset_pathname,
                phenomenon_name,
                property_set_name,
                elevation_layer_name);
            ElevationArray elevation_array{
                read<policy::read_into::DefaultValuePolicies<ElevationElement>, ElevationElement, rank>(
                    policy::read_into::DefaultValuePolicies<ElevationElement>{},
                    array_pathname,
                    partition_shape,
                    object_id)};

            array_pathname = fmt::format(
                "{}/{}/{}/{}",
                input_dataset_pathname,
                phenomenon_name,
                property_set_name,
                flow_direction_layer_name);
            FlowDirectionArray flow_direction_array{read<
                policy::read_into::DefaultValuePolicies<FlowDirectionElement>,
                FlowDirectionElement,
                rank>(
                policy::read_into::DefaultValuePolicies<FlowDirectionElement>{},
                array_pathname,
                partition_shape,
                object_id)};

            // -------------------------------------------------------------------------
            // Call operations for characterizing the flow direction network

            using InflowCountElement = std::uint8_t;
            using InflowCountPolicies = policy::OutputPolicies<policy::MarkNoDataByValue<InflowCountElement>>;
            using FlowDirectionPolicies =
                policy::InputPolicies<policy::DetectNoDataByValue<FlowDirectionElement>>;
            using InflowCountOperationPolicies = policy::Policies<
                policy::OutputsPolicies<InflowCountPolicies>,
                policy::InputsPolicies<FlowDirectionPolicies>>;

            InflowCountPolicies inflow_count_policies{policy::MarkNoDataByValue<InflowCountElement>{}};
            FlowDirectionPolicies flow_direction_policies{
                policy::DetectNoDataByValue<FlowDirectionElement>{}};

            InflowCountOperationPolicies inflow_count_operation_policies{
                inflow_count_policies, flow_direction_policies};
            auto inflow_count_array = lue::inflow_count<InflowCountOperationPolicies, InflowCountElement>(
                inflow_count_operation_policies, flow_direction_array);

            using BooleanElement = std::uint8_t;
            using CountElement = std::uint64_t;
            using StreamPolicies =
                lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<BooleanElement>>;
            using FlowDirectionPolicies =
                lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;
            using InterPartitionStreamOperationPolicies = lue::policy::Policies<
                lue::policy::OutputsPolicies<StreamPolicies>,
                lue::policy::InputsPolicies<FlowDirectionPolicies>>;

            StreamPolicies stream_policies{lue::policy::MarkNoDataByValue<BooleanElement>{}};
            // FlowDirectionPolicies
            // flow_direction_policies{lue::policy::DetectNoDataByValue<FlowDirectionElement>{}};

            InterPartitionStreamOperationPolicies inter_partition_stream_operation_policies{
                stream_policies, flow_direction_policies};

            auto const [stream_array, partition_io] =
                inter_partition_stream<InterPartitionStreamOperationPolicies, BooleanElement, CountElement>(
                    inter_partition_stream_operation_policies, flow_direction_array);

            // Export partition IDs
            // TODO

            // Aggregate information per partition
            // TODO

            // -------------------------------------------------------------------------
            // Write information to a new dataset

            // Create new dataset and add output rasters to it
            {
                auto output_dataset_ptr =
                    std::make_shared<ldm::Dataset>(ldm::create_dataset(output_dataset_pathname));

                auto output_raster_view = ldm::constant::create_raster_view(
                    output_dataset_ptr, phenomenon_name, property_set_name, grid_shape, space_box);
                object_id = output_raster_view.object_id();

                // Original flow direction (round-trip from partitioned array)
                output_raster_view.add_layer<ElevationElement>("elevation");
                output_raster_view.add_layer<FlowDirectionElement>("flow_direction");
                output_raster_view.add_layer<InflowCountElement>("inflow_count");
                output_raster_view.add_layer<BooleanElement>("inter_partition_stream");
            }

            {
                std::string array_pathname;
                std::vector<hpx::future<void>> arrays_written;

                array_pathname = fmt::format(
                    "{}/{}/{}/{}", output_dataset_pathname, phenomenon_name, property_set_name, "elevation");
                arrays_written.push_back(write_into(elevation_array, array_pathname, object_id));

                array_pathname = fmt::format(
                    "{}/{}/{}/{}",
                    output_dataset_pathname,
                    phenomenon_name,
                    property_set_name,
                    "flow_direction");
                arrays_written.push_back(write_into(flow_direction_array, array_pathname, object_id));

                array_pathname = fmt::format(
                    "{}/{}/{}/{}",
                    output_dataset_pathname,
                    phenomenon_name,
                    property_set_name,
                    "inflow_count");
                arrays_written.push_back(write_into(inflow_count_array, array_pathname, object_id));

                array_pathname = fmt::format(
                    "{}/{}/{}/{}",
                    output_dataset_pathname,
                    phenomenon_name,
                    property_set_name,
                    "inter_partition_stream");
                arrays_written.push_back(write_into(stream_array, array_pathname, object_id));

                // Wait for all writes to have happened
                lue::detail::when_all_get(move(arrays_written)).get();
            }

            return 0;
        }

    }  // namespace utility
}  // namespace lue
