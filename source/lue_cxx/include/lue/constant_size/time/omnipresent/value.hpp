#pragma once
#include "lue/constant_size/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Value:
    public constant_size::Value
{

public:

                   Value               ()=default;

                   Value               (Value const& other)=delete;

                   Value               (Value&& other)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const& other)=delete;

    Value&         operator=           (Value&& other)=default;

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
