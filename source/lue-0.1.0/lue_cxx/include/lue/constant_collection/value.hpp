#pragma once
#include "lue/value.hpp"


namespace lue {
namespace constant_collection {

class Value:
    public lue::Value
{

public:

                   Value               ()=default;

                   Value               (Value const& other)=delete;

                   Value               (Value&& other)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const& other)=delete;

    Value&         operator=           (Value&& other)=default;

};

} // namespace constant_collection
} // namespace lue
