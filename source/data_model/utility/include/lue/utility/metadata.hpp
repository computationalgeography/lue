#pragma once
#include "lue/core/clock.hpp"
#include "lue/core/time/unit.hpp"
#include "lue/utility/export.hpp"
#include <istream>
#include <nlohmann/json.hpp>


namespace lue {
    namespace utility {

        using JSON = nlohmann::json;
        using JSONPointer = JSON::json_pointer;
        using JSONCIterator = JSON::const_iterator;


        namespace json {

            LUE_UTILITY_EXPORT auto has_key(JSON const& object, std::string const& name) -> bool;

            LUE_UTILITY_EXPORT auto has_key(JSON const& object, JSONPointer const& pointer) -> bool;

            LUE_UTILITY_EXPORT auto object(JSON const& object, std::string const& name) -> JSON;

            LUE_UTILITY_EXPORT auto object(JSON const& object, JSONPointer const& pointer) -> JSON;

            LUE_UTILITY_EXPORT auto object(
                JSON const& object,
                JSONPointer const& pointer,
                std::string const& key,
                std::string const& value) -> JSON;

            LUE_UTILITY_EXPORT auto object(
                JSON const& object, std::string const& key, std::string const& value) -> JSON;

            LUE_UTILITY_EXPORT auto string(JSON const& object, std::string const& name) -> std::string;

            LUE_UTILITY_EXPORT auto string(JSON const& object, JSONPointer const& pointer) -> std::string;

            LUE_UTILITY_EXPORT auto boolean(JSON const& object, JSONPointer const& pointer) -> bool;

            LUE_UTILITY_EXPORT auto pointer(JSON const& object, std::string const& name) -> JSONPointer;

            LUE_UTILITY_EXPORT auto find(JSON const& object, std::string const& key, std::string const& value)
                -> JSONCIterator;

            LUE_UTILITY_EXPORT auto clock(JSON const& object) -> data_model::Clock;

        }  // namespace json


        /*!
            @brief      Class for representing a collection of metadata items
        */
        class LUE_UTILITY_EXPORT Metadata
        {

            public:

                Metadata() = default;

                explicit Metadata(std::string const& pathname);

                explicit Metadata(std::istream& stream);

                Metadata(Metadata&& other) = default;

                Metadata(Metadata const& other) = delete;

                ~Metadata() = default;

                auto operator=(Metadata&& other) -> Metadata& = default;

                auto operator=(Metadata const& other) -> Metadata& = delete;

                auto object() const -> JSON const&;

                auto string(JSONPointer const& pointer, std::string const& default_value) const
                    -> std::string;

                auto string(
                    std::string const& list_name,
                    std::string const& key,
                    std::string const& value,
                    JSONPointer const& pointer,
                    std::string const& default_value) const -> std::string;

                auto string(
                    std::string const& list_name,
                    std::string const& key,
                    std::string const& value,
                    JSONPointer const& pointer,
                    std::string const& value_key,
                    std::string const& default_value) const -> std::string;

                auto boolean(JSONPointer const& pointer, bool default_value) const -> bool;

            private:

                JSON _json;
        };

    }  // namespace utility


    namespace data_model::time {

        LUE_UTILITY_EXPORT void from_json(utility::JSON const& object, Unit& unit);

    }  // namespace data_model::time

}  // namespace lue
