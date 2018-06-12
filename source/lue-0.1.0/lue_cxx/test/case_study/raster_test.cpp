#define BOOST_TEST_MODULE lue case_study raster
#include <boost/test/unit_test.hpp>
#include "lue/lue.hpp"
#include "lue/test.hpp"
#include <numeric>


// template<
//     typename C>
// std::size_t nr_cells_in_rasters(
//     C const& collection,
//     std::size_t const rank)
// {
//     using T = typename C::value_type;
// 
//     std::size_t result = 0;
//     assert(collection.size() % rank == 0);
// 
//     // For each field, calculate the product of the number of cells
//     // in each dimension. Sum all these products to end up with the
//     // number of cells in all rasters.
//     for(auto it = collection.begin(); it != collection.end(); it += rank) {
//         result += std::accumulate(it, it + rank, 1, std::multiplies<T>());
//     }
// 
//     return result;
// }


BOOST_AUTO_TEST_CASE(create_and_read_raster)
{
    namespace omnipresent = lue::constant_collection::time::omnipresent;

    std::string const dataset_name = "areas.lue";
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "areas";

    hsize_t const nr_areas = 10;
    std::vector<hsize_t> const ids{2, 4, 6, 8, 10, 9, 7, 5, 3, 1};

    using CoordinateType = double;
    lue::hdf5::Datatype const coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<CoordinateType>::type_id()};
    std::size_t const rank = 2;
    std::vector<CoordinateType> boxes(nr_areas * 2 * rank);
    std::iota(boxes.begin(), boxes.end(), 5);

    std::string const discretization_property_name = "space_discretization";
    using CountType = hsize_t;
    lue::hdf5::Datatype const count_datatype{
        lue::hdf5::NativeDatatypeTraits<CountType>::type_id()};
    std::vector<CountType> counts(nr_areas * rank);
    std::iota(counts.begin(), counts.end(), 9);

    // auto const nr_cells = nr_cells_in_rasters(counts, rank);

    std::string const elevation_property_name = "elevation";
    using ValueType = float;
    lue::hdf5::Datatype const value_datatype{
        lue::hdf5::NativeDatatypeTraits<ValueType>::type_id()};
    std::vector<std::vector<ValueType>> elevation_fields(nr_areas);
    for(std::size_t i = 0; i < nr_areas; ++i) {
        auto nr_cells = std::accumulate(
            counts.begin() + (i * rank), counts.begin() + ((i + 1) * rank),
            1, std::multiplies<std::size_t>());
        elevation_fields[i].resize(nr_cells);
        std::iota(elevation_fields[i].begin(), elevation_fields[i].end(), i);
    }

    // std::iota(elevation_fields.begin(), elevation_fields.end(), 99);

    lue::test::DatasetFixture fixture{dataset_name};

    // Create
    {
        auto dataset = lue::create_dataset(dataset_name);
        auto& phenomenon = dataset.add_phenomenon(phenomenon_name);
        auto areas = lue::constant_collection::create_property_set(
            phenomenon, property_set_name);

        // Ids
        {
            auto& ids_ = areas.ids();
            ids_.reserve(nr_areas);
            ids_.write(ids.data());
        }

        // Domain
        {
            // Space domain
            {
                auto space_domain = omnipresent::create_space_box_domain(
                    areas, coordinate_datatype, rank);
                auto& boxes_ = space_domain.items();
                boxes_.reserve(nr_areas);
                boxes_.write(boxes.data());
            }
        }

        // Properties
        {

            // Field property
            {
                auto elevation = omnipresent::different_shape::create_property(
                    areas, elevation_property_name, value_datatype, rank);
                auto& elevation_fields_ = elevation.values();
                elevation_fields_.reserve(nr_areas, counts.data());
                for(hsize_t i = 0; i < nr_areas; ++i) {
                    elevation_fields_.write(i, elevation_fields[i].data());
                }

                // Discretization property
                {
                    auto discretization = omnipresent::same_shape::create_property(
                        areas, discretization_property_name, count_datatype,
                        lue::hdf5::Shape{rank});
                    auto& counts_ = discretization.values();
                    counts_.reserve(nr_areas);
                    counts_.write(counts.data());

                    elevation.discretize_space(discretization);
                }
            }
        }
    }

    lue::assert_is_valid(dataset_name);

    // Read
    {
        auto const dataset = lue::Dataset(dataset_name);
        auto const& phenomenon = dataset.phenomena()[phenomenon_name];
        auto const& property_set = phenomenon.property_sets()[property_set_name];

        BOOST_CHECK(
            property_set.configuration().type<lue::Occurrence>() ==
            lue::Occurrence::continuous);

        lue::constant_collection::PropertySet areas{property_set.id()};

        // Ids
        {
            BOOST_CHECK_EQUAL(areas.ids().nr_items(), nr_areas);

            std::vector<hsize_t> ids_(nr_areas);
            areas.ids().read(ids_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                ids_.begin(), ids_.end(),
                ids.begin(), ids.end());
        }

        // Domain
        {
            auto const& domain = areas.domain();

            // Time domain
            {
                BOOST_CHECK(!time_domain_exists(domain));
            }

            // Space domain
            {
                BOOST_CHECK(space_domain_exists(domain));

                lue::SpaceDomain const space_domain{domain};
                auto const& configuration{space_domain.configuration()};

                BOOST_CHECK(
                    configuration.type<lue::CollectionVariability>() ==
                    lue::CollectionVariability::constant);
                BOOST_CHECK(
                    configuration.type<lue::Mobility>() ==
                    lue::Mobility::stationary);
                BOOST_CHECK(
                    configuration.type<lue::SpaceDomainItemType>() ==
                    lue::SpaceDomainItemType::box);

                omnipresent::SpaceBoxDomain space_box_domain{
                    lue::constant_collection::SpaceDomain{domain}};

                BOOST_CHECK_EQUAL(
                    space_box_domain.items().nr_items(), nr_areas);

                std::vector<CoordinateType> boxes_(nr_areas * 2 * rank);

                space_box_domain.items().read(boxes_.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    boxes_.begin(), boxes_.end(),
                    boxes.begin(), boxes.end());
            }
        }

        // Properties
        {

            // Field property
            {
                auto const& property =
                    areas.properties()[elevation_property_name];
                auto const configuration = property.configuration();

                BOOST_CHECK(
                    configuration.type<lue::Occurrence>() ==
                    lue::Occurrence::continuous);
                BOOST_CHECK(
                    configuration.type<lue::ShapeVariability>() ==
                    lue::ShapeVariability::constant);
                BOOST_CHECK(
                    configuration.type<lue::ShapePerItem>() ==
                    lue::ShapePerItem::different);
                BOOST_CHECK(
                    configuration.type<lue::CollectionVariability>() ==
                    lue::CollectionVariability::constant);

                BOOST_CHECK(property.space_is_discretized());

                BOOST_CHECK(
                    omnipresent::different_shape::Property::file_datatype(
                        lue::hdf5::Group{property.id()}) ==
                    lue::hdf5::Datatype{H5T_IEEE_F32LE});

                omnipresent::different_shape::Property elevation_property{
                    lue::hdf5::Group{property.id()}, value_datatype};

                BOOST_CHECK_EQUAL(
                    elevation_property.values().nr_items(), nr_areas);

                for(std::size_t i = 0; i < nr_areas; ++i) {
                    lue::hdf5::Shape shape_written{
                        counts.begin() + (i * rank),
                        counts.begin() + ((i + 1) * rank)};
                    auto const shape_read{
                        elevation_property.values().value_shape(i)};
                    BOOST_CHECK(shape_read == shape_written);

                    auto const& values_written{elevation_fields[i]};
                    std::vector<ValueType> values_read(
                        std::accumulate(shape_read.begin(), shape_read.end(),
                            1, std::multiplies<hsize_t>()));
                    elevation_property.values().read(i, values_read.data());

                    BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        values_written.begin(), values_written.end());
                }

                // Discretization property
                {
                    auto const& property =
                        elevation_property.space_discretization();
                    BOOST_CHECK_EQUAL(
                        property.id().name(),
                        lue::space_discretization_tag);
                    auto const configuration =
                        property.configuration();

                    BOOST_CHECK(
                        configuration.type<lue::Occurrence>() ==
                        lue::Occurrence::continuous);
                    BOOST_CHECK(
                        configuration.type<lue::ShapeVariability>() ==
                        lue::ShapeVariability::constant);
                    BOOST_CHECK(
                        configuration.type<lue::ShapePerItem>() ==
                        lue::ShapePerItem::same);
                    BOOST_CHECK(
                        configuration.type<lue::CollectionVariability>() ==
                        lue::CollectionVariability::constant);

                    BOOST_CHECK(
                        omnipresent::same_shape::Property::file_datatype(
                            lue::hdf5::Group{property.id()}) ==
                        lue::hdf5::Datatype{
                            lue::hdf5::StandardDatatypeTraits<CountType>::
                                type_id()});

                    omnipresent::same_shape::Property discretization_property{
                        lue::hdf5::Group{property.id()}, count_datatype};

                    BOOST_CHECK_EQUAL(
                        discretization_property.values().nr_items(), nr_areas);

                    std::vector<CountType> values_read(nr_areas * rank);
                    discretization_property.values().read(values_read.data());

                    BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        counts.begin(), counts.end());
                }
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(create_and_read_raster2)
{
    // - property_set
    //     - time
    //         - N/A (continuous)
    //         - ContinuousPropertySet
    //     - space
    //         - extent: stationary/Box
    // - property
    //     - different_shape/ContinuousProperty
    //     - discretization
    //         - regular grid: same_shape/ContinuousProperty

    std::string const dataset_name = "areas.lue";
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "areas";
    std::string const elevation_property_name = "elevation";
    std::string const discretization_property_name = "space_discretization";

    hsize_t const nr_areas = 10;
    std::vector<hsize_t> const id{2, 4, 6, 8, 10, 9, 7, 5, 3, 1};

    // Coordinates of extents
    using CoordinateType = double;
    lue::hdf5::Datatype const coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<CoordinateType>::type_id()};
    std::size_t const rank = 2;
    std::vector<CoordinateType> boxes(nr_areas * 2 * rank);
    std::iota(boxes.begin(), boxes.end(), 5);

    // Shape of grids
    using CountType = hsize_t;
    lue::hdf5::Datatype const count_datatype{
        lue::hdf5::NativeDatatypeTraits<CountType>::type_id()};
    std::vector<CountType> counts(nr_areas * rank);
    std::iota(counts.begin(), counts.end(), 9);

    // Grid cell values
    using ValueType = float;
    lue::hdf5::Datatype const value_datatype{
        lue::hdf5::NativeDatatypeTraits<ValueType>::type_id()};
    std::vector<std::vector<ValueType>> elevation_fields(nr_areas);
    for(std::size_t i = 0; i < nr_areas; ++i) {
        auto nr_cells = std::accumulate(
            counts.begin() + (i * rank), counts.begin() + ((i + 1) * rank),
            1, std::multiplies<std::size_t>());
        elevation_fields[i].resize(nr_cells);
        std::iota(elevation_fields[i].begin(), elevation_fields[i].end(), i);
    }

    lue::test::DatasetFixture fixture{dataset_name};

    // Create
    {
        auto dataset = lue::create_dataset(dataset_name);
        auto& phenomenon = dataset.add_phenomenon(phenomenon_name);
        auto areas = phenomenon.add_continuous_property_set(
            property_set_name);

        // Ids
        {
            areas.id().reserve(nr_areas);
            areas.id().write(id.data());
        }

        // Domain
        {
            auto& domain = areas.domain();

            // Space domain
            {
                auto space_domain =
                    lue::create_space_domain(domain,
                        lue::SpaceConfiguration{
                            lue::Occurrence::continuous,
                            lue::CollectionVariability::constant,
                            lue::Mobility::stationary,
                            lue::SpaceDomainItemType::box
                        }
                    );

                auto box = lue::stationary::create_box(
                    space_domain, coordinate_datatype, rank);

                box.reserve(nr_areas);
                box.write(boxes.data());
            }
        }

        // Properties
        {

            // Field property
            {
                auto elevation =
                    lue::different_shape::create_continuous_property(
                        areas, elevation_property_name, value_datatype,
                        rank);

                elevation.value().reserve(nr_areas, counts.data());

                for(hsize_t i = 0; i < nr_areas; ++i) {
                    elevation.value().write(i, elevation_fields[i].data());
                }

                // Discretization property
                {
                    auto discretization =
                        lue::same_shape::create_continuous_property(
                        areas, discretization_property_name, count_datatype,
                        lue::hdf5::Shape{rank});

                    discretization.value().reserve(nr_areas);
                    discretization.value().write(counts.data());

                    elevation.discretize_space(discretization);
                }
            }
        }
    }

    lue::assert_is_valid(dataset_name);

    // Read
    {
        auto const dataset = lue::Dataset(dataset_name);
        auto const& phenomenon = dataset.phenomena()[phenomenon_name];
        auto const& property_set = phenomenon.property_sets()[property_set_name];

        BOOST_CHECK(
            property_set.configuration().type<lue::Occurrence>() ==
            lue::Occurrence::continuous);

        lue::ContinuousPropertySet areas{property_set.id()};

        // Ids
        {
            BOOST_CHECK_EQUAL(areas.id().nr_items(), nr_areas);

            std::vector<hsize_t> id_(nr_areas);
            areas.id().read(id_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                id_.begin(), id_.end(),
                id.begin(), id.end());
        }

        // Domain
        {
            auto const& domain = areas.domain();

            // Time domain
            {
                BOOST_CHECK(!time_domain_exists(domain));
            }

            // Space domain
            {
                BOOST_CHECK(space_domain_exists(domain));

                lue::SpaceDomain const space_domain{domain};
                auto const& configuration{space_domain.configuration()};

                BOOST_CHECK(
                    configuration.type<lue::Occurrence>() ==
                    lue::Occurrence::continuous);
                BOOST_CHECK(
                    configuration.type<lue::CollectionVariability>() ==
                    lue::CollectionVariability::constant);
                BOOST_CHECK(
                    configuration.type<lue::Mobility>() ==
                    lue::Mobility::stationary);
                BOOST_CHECK(
                    configuration.type<lue::SpaceDomainItemType>() ==
                    lue::SpaceDomainItemType::box);

                lue::stationary::Box box{space_domain};

                BOOST_CHECK(box.memory_datatype() == coordinate_datatype);

                BOOST_CHECK_EQUAL(box.nr_items(), nr_areas);

                std::vector<CoordinateType> boxes_(nr_areas * 2 * rank);

                box.read(boxes_.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    boxes_.begin(), boxes_.end(),
                    boxes.begin(), boxes.end());
            }
        }

        // Properties
        {

            // Field property
            {
                auto const& property =
                    areas.properties()[elevation_property_name];

                BOOST_CHECK(
                    property.configuration().type<lue::Occurrence>() ==
                    lue::Occurrence::continuous);
                BOOST_CHECK(
                    property.configuration().type<lue::ShapeVariability>() ==
                    lue::ShapeVariability::constant);
                BOOST_CHECK(
                    property.configuration().type<lue::ShapePerItem>() ==
                    lue::ShapePerItem::different);
                BOOST_CHECK(
                    property.configuration().type<lue::CollectionVariability>() ==
                    lue::CollectionVariability::constant);
                BOOST_CHECK(property.space_is_discretized());

                lue::different_shape::ContinuousProperty elevation_property{
                    areas.properties(), elevation_property_name};

                BOOST_CHECK(
                    elevation_property.value().memory_datatype() ==
                    value_datatype);
                BOOST_CHECK_EQUAL(
                    elevation_property.value().nr_items(), nr_areas);

                for(std::size_t i = 0; i < nr_areas; ++i) {
                    lue::hdf5::Shape shape_written{
                        counts.begin() + (i * rank),
                        counts.begin() + ((i + 1) * rank)};
                    auto const shape_read{
                        elevation_property.value().value_shape(i)};
                    BOOST_CHECK(shape_read == shape_written);

                    auto const& values_written{elevation_fields[i]};
                    std::vector<ValueType> values_read(
                        std::accumulate(shape_read.begin(), shape_read.end(),
                            1, std::multiplies<hsize_t>()));
                    elevation_property.value().read(i, values_read.data());

                    BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        values_written.begin(), values_written.end());
                }

                // Discretization property
                {
                    auto const& property =
                        elevation_property.space_discretization();

                    BOOST_CHECK(
                        property.configuration().type<lue::Occurrence>() ==
                        lue::Occurrence::continuous);
                    BOOST_CHECK(
                        property.configuration().type<lue::ShapeVariability>() ==
                        lue::ShapeVariability::constant);
                    BOOST_CHECK(
                        property.configuration().type<lue::ShapePerItem>() ==
                        lue::ShapePerItem::same);
                    BOOST_CHECK(
                        property.configuration().type<
                            lue::CollectionVariability>() ==
                                lue::CollectionVariability::constant);

                    lue::same_shape::ContinuousProperty discretization_property{
                        property.id()};
                    BOOST_CHECK(
                        discretization_property.value().memory_datatype() ==
                        count_datatype);
                    BOOST_CHECK_EQUAL(
                        discretization_property.value().nr_items(), nr_areas);

                    std::vector<CountType> values_read(nr_areas * rank);
                    discretization_property.value().read(values_read.data());

                    BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        counts.begin(), counts.end());
                }
            }
        }
    }
}
