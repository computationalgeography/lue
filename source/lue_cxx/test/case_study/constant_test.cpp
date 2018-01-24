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

    // Create
    {
        // Dataset
        lue::test::DatasetFixture fixture{dataset_name, false};
        auto dataset = lue::create_dataset(dataset_name);

        // Phenomenon
        auto& planets = dataset.add_phenomenon(phenomenon_name);

        // Property-set
        auto constants =
            omnipresent::create_property_set(planets, property_set_name);

        hsize_t const nr_planets = 3;

        auto& ids = constants.ids();
        ids.reserve(nr_planets);
        std::vector<hsize_t> ids_ = {4, 29, 13};
        ids.write(ids_.data());

        // Property
        auto gravity =
            omnipresent::same_shape::create_property(
                constants, property_name,
                lue::hdf5::Datatype{H5T_NATIVE_FLOAT});

        auto& values = gravity.values();
        values.reserve(nr_planets);
        std::vector<float> gravity_ = {1.5, 2.5, 3.5};
        values.write(gravity_.data());
    }

    lue::assert_is_valid(dataset_name);

    // Read
    {
        // // Dataset
        // auto dataset = lue::Dataset(dataset_name);

        // // Phenomenon
        // auto& planets = dataset.phenomena()[phenomenon_name];

        // // Property-set
        // auto& constants = planets.property_sets()[property_set_name];

        // // Property
        // auto& gravity = constants.properties()[property_name];



        // // TODO By-value return
        // auto planets = dataset.phenomena()[phenomenon_name];

        // // TODO By-value return
        // // TODO Wrapped omnipresent::PropertySet, with interface that all
        // //      property-sets support
        // auto constants = planets.property_sets()[property_set_name];

        // // TODO By-value return
        // // TODO Wrapped omnipresent::same_shape::Value, with interface that
        // //      all values support
        // auto gravity = constants.properties()[property_name];


    }
}
