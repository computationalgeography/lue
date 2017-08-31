#define BOOST_TEST_MODULE lue constant_size time omnipresent property_set
#include <boost/test/unit_test.hpp>
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/dataset.hpp"
#include <iostream>


namespace {

class Fixture
{

public:

    Fixture(
        std::string const& dataset_name)

        : _dataset_name(dataset_name)

    {
        // Guarantees:
        // - Dataset does not exist after setup
        remove_dataset();

        BOOST_REQUIRE(!lue::dataset_exists(_dataset_name));
    }


    ~Fixture()
    {
        // Guarantees:
        // - Dataset does not exist after teardown
        // remove_dataset();

        // BOOST_CHECK(!lue::dataset_exists(_dataset_name));
    }


private:

    std::string const _dataset_name;

    void remove_dataset()
    {
        if(lue::dataset_exists(_dataset_name)) {
            lue::remove_dataset(_dataset_name);
        }
    }

};

}  // Anonymous namespace


namespace omnipresent = lue::constant_size::time::omnipresent;


BOOST_AUTO_TEST_CASE(create_new_property_set)
{
    std::string const dataset_name = "create_new_property_set.lue";
    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";
    Fixture f(dataset_name);

    auto dataset = lue::create_dataset(dataset_name);
    auto& phenomenon = dataset.add_phenomenon(phenomenon_name);

    BOOST_CHECK_EQUAL(phenomenon.property_sets().size(), 0);

    auto const property_set = omnipresent::create_property_set(
        phenomenon, property_set_name);

    BOOST_CHECK_EQUAL(phenomenon.property_sets().size(), 1);
    BOOST_CHECK(
        property_set.configuration().size_of_item_collection_type() ==
        lue::SizeOfItemCollectionType::constant_size);


    auto const& attributes = property_set.attributes();
    BOOST_CHECK(attributes.exists("lue_size_of_item_collection_type"));
    BOOST_CHECK_EQUAL(attributes.read<std::string>(
        "lue_size_of_item_collection_type"), "lue_constant_size");




}
