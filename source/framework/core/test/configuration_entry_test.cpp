#define BOOST_TEST_MODULE lue framework core configuration_entry
#include "lue/framework/core/configuration_entry.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(cast_vector_uint64)
{
    using Element = std::vector<std::uint64_t>;

    {
        std::string value{"[200, 300]"};
        auto element = lue::detail::cast<Element>(value);

        BOOST_REQUIRE_EQUAL(element.size(), 2);
        BOOST_CHECK_EQUAL(element[0], std::uint64_t{200});
        BOOST_CHECK_EQUAL(element[1], std::uint64_t{300});
    }

    {
        std::string value{"[200]"};
        auto element = lue::detail::cast<Element>(value);

        BOOST_REQUIRE_EQUAL(element.size(), 1);
        BOOST_CHECK_EQUAL(element[0], std::uint64_t{200});
    }

    {
        std::string value{"[]"};
        auto element = lue::detail::cast<Element>(value);

        BOOST_REQUIRE_EQUAL(element.size(), 0);
    }
}
