#define BOOST_TEST_MODULE lue case_study constant
#include <boost/test/unit_test.hpp>
#include "lue/lue.hpp"
#include "lue/test.hpp"


BOOST_AUTO_TEST_CASE(create_and_read_constant)
{
    namespace omnipresent = lue::constant_collection::time::omnipresent;

    std::string const dataset_name = "planets.lue";
    std::string const phenomenon_name = "planets";
    std::string const property_set_name = "constants";
    std::string const property_name = "gravity";

    hsize_t const nr_planets = 3;
    std::vector<hsize_t> const ids{4, 29, 13};

    using ValueType = float;
    lue::hdf5::Datatype const value_datatype{
        lue::hdf5::NativeDatatypeTraits<ValueType>::type_id()};
    std::vector<ValueType> gravity_values{1.5, 2.5, 3.5};

    lue::test::DatasetFixture fixture{dataset_name};

    // Create
    {
        auto dataset = lue::create_dataset(dataset_name);
        auto& planets = dataset.add_phenomenon(phenomenon_name);
        auto constants = lue::constant_collection::create_property_set(
            planets, property_set_name);

        // Ids
        {
            auto& ids_ = constants.ids();
            ids_.reserve(nr_planets);
            ids_.write(ids.data());
        }

        // Property
        {
            auto gravity =
                omnipresent::same_shape::create_property(
                    constants, property_name, value_datatype);
            auto& gravity_values_ = gravity.values();
            gravity_values_.reserve(nr_planets);
            gravity_values_.write(gravity_values.data());
        }
    }

    lue::assert_is_valid(dataset_name);

    // Read
    {
        auto const dataset = lue::Dataset(dataset_name);
        auto const& planets = dataset.phenomena()[phenomenon_name];
        auto const& property_set = planets.property_sets()[property_set_name];

        BOOST_CHECK(
            property_set.configuration().type<lue::Occurrence>() ==
            lue::Occurrence::continuous);

        lue::constant_collection::PropertySet constants{property_set.id()};

        // Ids
        {
            BOOST_CHECK_EQUAL(constants.ids().nr_items(), nr_planets);

            std::vector<hsize_t> ids_(nr_planets);
            constants.ids().read(ids_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                ids_.begin(), ids_.end(),
                ids.begin(), ids.end());
        }

        // Domain
        {
            auto const& domain = constants.domain();

            // Time domain
            {
                BOOST_CHECK(!time_domain_exists(domain));
            }

            // Space domain
            {
                BOOST_CHECK(!space_domain_exists(domain));
            }
        }

        // Property
        {
            auto const& property = constants.properties()[property_name];

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
                property.configuration().type<lue::CollectionVariability>() ==
                lue::CollectionVariability::constant);

            BOOST_CHECK(
                omnipresent::same_shape::Property::file_datatype(
                    lue::hdf5::Group{property.id()}) ==
                lue::hdf5::Datatype{H5T_IEEE_F32LE});

            omnipresent::same_shape::Property gravity{
                lue::hdf5::Group{property.id()}, value_datatype};

            BOOST_CHECK_EQUAL(gravity.values().nr_items(), nr_planets);

            std::vector<ValueType> gravity_values_(nr_planets);
            gravity.values().read(gravity_values_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                gravity_values_.begin(), gravity_values_.end(),
                gravity_values.begin(), gravity_values.end());
        }
    }
}


BOOST_AUTO_TEST_CASE(create_and_read_constant2)
{
    // - property_set
    //     - time
    //         - N/A (continuous)
    //         - ContinuousPropertySet
    //     - space
    //         - N/A
    // - property
    //     - same_shape
    //     - continuous

    std::string const dataset_name = "planets.lue";
    std::string const phenomenon_name = "planets";
    std::string const property_set_name = "constants";
    std::string const property_name = "gravity";

    hsize_t const nr_planets = 3;
    std::vector<hsize_t> const id{4, 29, 13};

    using ValueType = float;
    lue::hdf5::Datatype const value_datatype{
        lue::hdf5::NativeDatatypeTraits<ValueType>::type_id()};
    std::vector<ValueType> const gravity_values{1.5, 2.5, 3.5};

    lue::test::DatasetFixture fixture{dataset_name};

    // Create
    {
        auto dataset = lue::create_dataset(dataset_name);
        auto& planets = dataset.add_phenomenon(phenomenon_name);
        auto constants = planets.add_continuous_property_set(
            property_set_name);

        // Ids
        {
            constants.id().reserve(nr_planets);
            constants.id().write(id.data());
        }

        // Property
        {
            auto gravity =
                lue::same_shape::create_continuous_property(
                    constants, property_name, value_datatype);

            gravity.value().reserve(nr_planets);
            gravity.value().write(gravity_values.data());
        }
    }

    lue::assert_is_valid(dataset_name);

    // Read
    {
        auto const dataset = lue::Dataset(dataset_name);
        auto const& planets = dataset.phenomena()[phenomenon_name];
        auto& property_set = planets.property_sets()[property_set_name];

        BOOST_CHECK(
            property_set.configuration().type<lue::Occurrence>() ==
            lue::Occurrence::continuous);

        lue::ContinuousPropertySet constants{property_set.id()};

        // Id
        {
            BOOST_CHECK_EQUAL(constants.id().nr_items(), nr_planets);

            std::vector<hsize_t> id_(nr_planets);
            constants.id().read(id_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                id_.begin(), id_.end(),
                id.begin(), id.end());
        }

        // Domain
        {
            auto const& domain = constants.domain();

            // Time domain
            {
                BOOST_CHECK(!time_domain_exists(domain));
            }

            // Space domain
            {
                BOOST_CHECK(!space_domain_exists(domain));
            }
        }

        // Property
        {
            auto const& property = constants.properties()[property_name];

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
                property.configuration().type<lue::CollectionVariability>() ==
                lue::CollectionVariability::constant);

            lue::same_shape::ContinuousProperty gravity{
                constants.properties(), property_name};

            BOOST_CHECK(gravity.value().memory_datatype() == value_datatype);
            BOOST_CHECK_EQUAL(gravity.value().nr_items(), nr_planets);

            std::vector<ValueType> gravity_values_(nr_planets);

            gravity.value().read(gravity_values_.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                gravity_values_.begin(), gravity_values_.end(),
                gravity_values.begin(), gravity_values.end());
        }
    }
}
