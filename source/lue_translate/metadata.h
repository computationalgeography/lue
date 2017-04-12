#pragma once
#include <json.hpp>
#include <fstream>
#include <iostream>


namespace lue {
namespace utility {

/*!
    @brief      Class for representing a collection of metadata items
*/
class Metadata
{

public:

                   Metadata            ()=default;

                   Metadata            (std::string const& pathname);

                   Metadata            (std::istream& stream);

                   Metadata            (Metadata&& other)=default;

                   Metadata            (Metadata const& other)=delete;

                   ~Metadata           ()=default;

    Metadata&      operator=           (Metadata&& other)=default;

    Metadata&      operator=           (Metadata const& other)=delete;

    template<
        typename T>
    T              value               (std::string const& path,
                                        T const& default_value) const;

    std::string    value               (std::string const& path,
                                        std::string const& default_value) const;

private:

    nlohmann::json _json;

};


/*!
    @brief      Return the value pointed to by @a path, or @a
                default_value in case no such value exists at that
                location
    @tparam     T Type of value to obtain
*/
template<
    typename T>
inline T Metadata::value(
    std::string const& path,
    T const& default_value) const
{
    return _json.value(nlohmann::json::json_pointer(path), default_value);
}

}  // namespace utility
}  // namespace lue
