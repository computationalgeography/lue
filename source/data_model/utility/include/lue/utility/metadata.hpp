#pragma once
#include "lue/core/clock.hpp"
#include "lue/core/time/unit.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


namespace lue {
namespace utility {

using JSON = nlohmann::json;
using JSONPointer = JSON::json_pointer;
using JSONCIterator = JSON::const_iterator;


namespace json {

bool               has_key             (JSON const& object,
                                        std::string const& name);

bool               has_key             (JSON const& object,
                                        JSONPointer const& pointer);

JSON               object              (JSON const& object,
                                        std::string const& name);

JSON               object              (JSON const& object,
                                        JSONPointer const& pointer);

JSON               object              (JSON const& object,
                                        JSONPointer const& pointer,
                                        std::string const& key,
                                        std::string const& value);

std::string        string              (JSON const& object,
                                        std::string const& name);

std::string        string              (JSON const& object,
                                        JSONPointer const& pointer);

bool               boolean             (JSON const& object,
                                        JSONPointer const& pointer);

JSONPointer        pointer             (JSON const& object,
                                        std::string const& name);

JSONCIterator      find                (JSON const& object,
                                        std::string const& name,
                                        std::string const& string);

data_model::Clock  clock               (JSON const& object);

}  // namespace json


/*!
    @brief      Class for representing a collection of metadata items
*/
class Metadata
{

public:

                   Metadata            ()=default;

     explicit      Metadata            (std::string const& pathname);

     explicit      Metadata            (std::istream& stream);

                   Metadata            (Metadata&& other)=default;

                   Metadata            (Metadata const& other)=delete;

                   ~Metadata           ()=default;

    Metadata&      operator=           (Metadata&& other)=default;

    Metadata&      operator=           (Metadata const& other)=delete;

    JSON const&    object              () const;

    std::string    string              (JSONPointer const& pointer,
                                        std::string const& default_value)
                                            const;

    std::string    string              (std::string const& list_name,
                                        std::string const& key,
                                        std::string const& value,
                                        JSONPointer const& pointer,
                                        std::string const& default_value)
                                            const;

    std::string    string              (std::string const& list_name,
                                        std::string const& key,
                                        std::string const& value,
                                        JSONPointer const& pointer,
                                        std::string const& value_key,
                                        std::string const& default_value)
                                            const;

    bool           boolean             (JSONPointer const& pointer,
                                        bool const default_value) const;

private:

    JSON           _json;

};

}  // namespace utility


namespace data_model {
namespace time {

void               from_json           (utility::JSON const& object,
                                        Unit& unit);

}  // namespace time
}  // namespace data_model
}  // namespace lue
