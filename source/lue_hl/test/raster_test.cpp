#define BOOST_TEST_MODULE lue hl raster
#include <boost/test/unit_test.hpp>
#include "lue/hl/raster.hpp"


namespace {

// class Fixture
// {
// 
// public:
// 
//     Fixture(
//         std::string const& dataset_name)
// 
//         : _dataset_name(dataset_name)
// 
//     {
//         // Guarantees:
//         // - Dataset does not exist after setup
//         remove_dataset();
// 
//         BOOST_REQUIRE(!lue::dataset_exists(_dataset_name));
//     }
// 
// 
//     ~Fixture()
//     {
//         // Guarantees:
//         // - Dataset does not exist after teardown
//         remove_dataset();
// 
//         BOOST_CHECK(!lue::dataset_exists(_dataset_name));
//     }
// 
// 
// private:
// 
//     std::string const _dataset_name;
// 
//     void remove_dataset()
//     {
//         if(lue::dataset_exists(_dataset_name)) {
//             lue::remove_dataset(_dataset_name);
//         }
//     }
// 
// };

BOOST_AUTO_TEST_CASE(dummy)
{
    BOOST_CHECK(true);
}

}  // Anonymous namespace
