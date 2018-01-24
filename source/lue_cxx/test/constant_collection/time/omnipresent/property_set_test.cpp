#define BOOST_TEST_MODULE lue constant_collection time omnipresent property_set
#include <boost/test/unit_test.hpp>
#include "lue/constant_collection/time/omnipresent/property_set.hpp"
#include "lue/dataset.hpp"
#include "lue/test.hpp"
#include <iostream>


namespace omnipresent = lue::constant_collection::time::omnipresent;


BOOST_AUTO_TEST_CASE(create_new_property_set)
{
    std::string const dataset_name = "create_new_property_set.lue";
    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";

    lue::test::DatasetFixture fixture{dataset_name};

    auto dataset = lue::create_dataset(dataset_name);
    auto& phenomenon = dataset.add_phenomenon(phenomenon_name);

    BOOST_CHECK_EQUAL(phenomenon.property_sets().size(), 0);

    auto const property_set = omnipresent::create_property_set(
        phenomenon, property_set_name);

    BOOST_CHECK_EQUAL(phenomenon.property_sets().size(), 1);
    BOOST_CHECK(
        property_set.configuration().size_of_item_collection_type() ==
        lue::CollectionVariability::constant);


    auto const& attributes = property_set.attributes();
    BOOST_CHECK(attributes.exists("lue_size_of_item_collection_type"));
    BOOST_CHECK_EQUAL(attributes.read<std::string>(
        "lue_size_of_item_collection_type"), "lue_constant_collection");
}


BOOST_AUTO_TEST_CASE(share_item_ids)
{
    // Create two property-sets, sharing the collection of item ids.
    std::string const dataset_name = "share_item_ids.lue";
    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name1 = "my_property_set1";
    std::string const property_set_name2 = "my_property_set2";

    lue::test::DatasetFixture fixture{dataset_name};

    auto dataset = lue::create_dataset(dataset_name);
    auto& phenomenon = dataset.add_phenomenon(phenomenon_name);

    // Second property-set points to the same collection of item ids as
    // the first
    auto property_set1 = omnipresent::create_property_set(
        phenomenon, property_set_name1);
    auto property_set2 = omnipresent::create_property_set(
        phenomenon, property_set_name2, property_set1.ids());

    std::size_t const nr_items = 5;

    // Create and write item ids through first property-set and check whether
    // these ids can be read through the second property-set
    property_set1.ids().reserve(nr_items);

    std::vector<std::uint64_t> ids_written = { 5, 4, 3, 2, 1 };
    std::vector<std::uint64_t> ids_read(nr_items);

    auto const memory_dataspace =
        lue::hdf5::create_dataspace(lue::hdf5::Shape{nr_items});

    property_set1.ids().write(memory_dataspace, ids_written.data());
    property_set2.ids().read(memory_dataspace, ids_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
            ids_read.begin(), ids_read.end(),
            ids_written.begin(), ids_written.end());

    // Write item ids through second property-set and check whether
    // these ids can be read through the first property-set
    ids_written = { 15, 14, 13, 12, 11 };

    property_set2.ids().write(memory_dataspace, ids_written.data());
    property_set1.ids().read(memory_dataspace, ids_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
            ids_read.begin(), ids_read.end(),
            ids_written.begin(), ids_written.end());
}
