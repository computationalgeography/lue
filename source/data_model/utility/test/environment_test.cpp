#define BOOST_TEST_MODULE lue utility environment
#include <boost/test/unit_test.hpp>
#include "lue/utility/environment.hpp"


BOOST_AUTO_TEST_CASE(expand_environment_variables)
{
    auto expand =
        [](std::string const& string)
        {
            return lue::expand_environment_variables(string);
        };

    BOOST_CHECK_EQUAL(expand(""), "");

    std::string existing_variable_name;
    std::string existing_variable_value;
    std::string unexisting_variable_name;

    {
        for(std::string variable_name: {"HOME", "SHELL", "TMP", "TEMP"})
        {
            char const* const variable_value_ptr{std::getenv(variable_name.c_str())};

            if(variable_value_ptr != nullptr)
            {
                existing_variable_name = variable_name;
                existing_variable_value = std::string{variable_value_ptr};
                break;
            }
        }

        BOOST_REQUIRE(!existing_variable_name.empty());

        unexisting_variable_name = "MEH";
        char const* const unexisting_variable_ptr{std::getenv(unexisting_variable_name.c_str())};
        BOOST_REQUIRE(unexisting_variable_ptr == nullptr);
    }

    // Existing variable
    {
        BOOST_CHECK_EQUAL(
            expand("${" + existing_variable_name + "}"),
            existing_variable_value);
        BOOST_CHECK_EQUAL(
            expand("${" + existing_variable_name + "}/.."),
            existing_variable_value + "/..");
        BOOST_CHECK_EQUAL(
            expand("../${" + existing_variable_name + "}"),
            "../" + existing_variable_value);
        BOOST_CHECK_EQUAL(
            expand("../${" + existing_variable_name + "}/.."),
            "../" + existing_variable_value + "/..");
    }

    // Unexisting variable
    {
        BOOST_CHECK_EQUAL(
            expand("${" + unexisting_variable_name + "}"),
            "");
        BOOST_CHECK_EQUAL(expand(
            "${" + unexisting_variable_name + "}/.."),
            "/..");
        BOOST_CHECK_EQUAL(
            expand("../${" + unexisting_variable_name + "}"),
            "../");
        BOOST_CHECK_EQUAL(
            expand("../${" + unexisting_variable_name + "}/.."),
            "..//..");
    }

    // Combinations
    {
        BOOST_CHECK_EQUAL(
            expand("${" + existing_variable_name + "}${" +
                unexisting_variable_name + "}${" +
                existing_variable_name + "}"),
            existing_variable_value + existing_variable_value);
        BOOST_CHECK_EQUAL(
            expand("${" + unexisting_variable_name + "}${" +
                existing_variable_name + "}${" +
                unexisting_variable_name + "}"),
            existing_variable_value);
    }
}
