#pragma once
#include <hdf5.h>


namespace lue {

class Value
{

public:

                   Value               ()=default;

                   Value               (Value const& other)=delete;

                   Value               (Value&& other)=default;

    virtual        ~Value              ()=default;

    Value&         operator=           (Value const& other)=delete;

    Value&         operator=           (Value&& other)=default;

    virtual hsize_t nr_items           () const=0;

};

} // namespace lue
