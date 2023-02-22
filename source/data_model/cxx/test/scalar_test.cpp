#define BOOST_TEST_MODULE lue scalar
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_CASE(create, lue::data_model::test::DatasetFixture)
{
    std::string const phenomenon_name = "planets";
    std::string const property_set_name = "constants";
    std::string const property_name = "gravity";

    lue::data_model::Count const nr_planets = 15;
    using ValueType = double;
    lue::hdf5::Datatype const datatype{lue::hdf5::NativeDatatypeTraits<ValueType>::type_id()};

    std::vector<lue::data_model::ID> ids(nr_planets);
    lue::data_model::test::generate_random_ids(ids);

    std::vector<ValueType> values(nr_planets);
    lue::data_model::test::generate_random_values(values, 5.0, 15.0);

    // Create and write
    {
        auto& planets = dataset().add_phenomenon(phenomenon_name);
        auto& constants = planets.property_sets().add(property_set_name);

        planets.object_id().expand(nr_planets);
        planets.object_id().write(ids.data());

        auto& gravity = constants.properties().add(property_name, datatype);
        auto& value = gravity.value();

        value.expand(nr_planets);
        value.write(values.data());
    }

    BOOST_CHECK_NO_THROW(lue::data_model::assert_is_valid(pathname()));

    // Open and read
    {
        auto const& planets = dataset().phenomena()[phenomenon_name];
        auto const& constants = planets.property_sets()[property_set_name];

        BOOST_REQUIRE(constants.properties().contains(property_name));

        auto shape_per_object = constants.properties().shape_per_object(property_name);
        auto value_variability = constants.properties().value_variability(property_name);

        BOOST_REQUIRE_EQUAL(shape_per_object, lue::data_model::ShapePerObject::same);
        BOOST_REQUIRE_EQUAL(value_variability, lue::data_model::ValueVariability::constant);

        auto const& properties = constants.properties().collection<lue::data_model::same_shape::Properties>();

        auto const& gravity = properties[property_name];
        auto const& value = gravity.value();

        BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_planets);
        BOOST_REQUIRE_EQUAL(value.memory_datatype(), datatype);

        std::vector<ValueType> values_read(nr_planets);
        value.read(values_read.data());

        BOOST_CHECK_EQUAL_COLLECTIONS(values_read.begin(), values_read.end(), values.begin(), values.end());
    }
}
