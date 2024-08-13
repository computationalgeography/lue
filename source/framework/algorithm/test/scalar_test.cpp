#define BOOST_TEST_MODULE lue framework algorithm scalar
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar{};
        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), Element{0});
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar{};
        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), Element{0});
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar{};
        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), "");
    }
}


BOOST_AUTO_TEST_CASE(copy_construct)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{scalar_from};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), Element{5});
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{scalar_from};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), Element{5});
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar_from{"5"};
        lue::Scalar<Element> scalar_to{scalar_from};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), "5");

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), "5");
    }
}


BOOST_AUTO_TEST_CASE(move_construct)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{std::move(scalar_from)};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(!scalar_from.value().valid());
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{std::move(scalar_from)};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(!scalar_from.value().valid());
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar_from{"5"};
        lue::Scalar<Element> scalar_to{std::move(scalar_from)};

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), "5");

        BOOST_CHECK(!scalar_from.value().valid());
    }
}


BOOST_AUTO_TEST_CASE(convert_construct)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar{5};

        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), Element{5});
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar{5};

        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), Element{5});
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar{"5"};

        BOOST_CHECK(scalar.value().valid());
        BOOST_CHECK(scalar.value().is_ready());
        BOOST_CHECK_EQUAL(scalar.value().get(), "5");
    }
}


BOOST_AUTO_TEST_CASE(copy_assign)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{};
        scalar_to = scalar_from;

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), Element{5});
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{};
        scalar_to = scalar_from;

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), Element{5});
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar_from{"5"};
        lue::Scalar<Element> scalar_to{};
        scalar_to = scalar_from;

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), "5");

        BOOST_CHECK(scalar_from.value().valid());
        BOOST_CHECK(scalar_from.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_from.value().get(), "5");
    }
}


BOOST_AUTO_TEST_CASE(move_assign)
{
    {
        using Element = std::uint8_t;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{};
        scalar_to = std::move(scalar_from);

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(!scalar_from.value().valid());
    }

    {
        using Element = float;

        lue::Scalar<Element> scalar_from{5};
        lue::Scalar<Element> scalar_to{};
        scalar_to = std::move(scalar_from);

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), Element{5});

        BOOST_CHECK(!scalar_from.value().valid());
    }

    {
        using Element = std::string;

        lue::Scalar<Element> scalar_from{"5"};
        lue::Scalar<Element> scalar_to{};
        scalar_to = std::move(scalar_from);

        BOOST_CHECK(scalar_to.value().valid());
        BOOST_CHECK(scalar_to.value().is_ready());
        BOOST_CHECK_EQUAL(scalar_to.value().get(), "5");

        BOOST_CHECK(!scalar_from.value().valid());
    }
}


BOOST_AUTO_TEST_CASE(continuation)
{
    using Element = std::string;

    lue::Scalar<Element> greating1{"Hello"};
    lue::Scalar<Element> greating2 =
        greating1.value().then([](auto const& value) -> Element { return value.get() + " " + "World!"; });

    // Make the layered value (a shared future) go out of scope (not necessary for the
    // test). Shared futures can be copied. greating1's future is copied into the continuation,
    // so the shared state will remain available for use in the lambda. greating2's value is
    // a different one.
    greating1 = lue::Scalar<Element>{};

    BOOST_CHECK_EQUAL(greating2.value().get(), Element{"Hello World!"});
}


// BOOST_AUTO_TEST_CASE(conversion)
// {
// }
