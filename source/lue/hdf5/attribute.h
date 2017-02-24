#pragma once
#include "lue/hdf5/identifier.h"
#include "lue/hdf5/dataspace.h"
#include "lue/hdf5/datatype.h"


namespace lue {
namespace hdf5 {

class Attribute
{

public:

//                    Attribute           (Identifier const& location,
//                                         std::string const& name,
//                                         Datatype const& datatype,
//                                         Dataspace const& dataspace);

                   Attribute           (Identifier&& id);

                   Attribute           (Identifier const& location,
                                        std::string const& name);

                   Attribute           (Attribute const& other)=delete;

                   Attribute           (Attribute&& other)=default;

                   ~Attribute          ()=default;

    Attribute&     operator=           (Attribute const& other)=delete;

    Attribute&     operator=           (Attribute&& other)=default;

    Identifier const& id               () const;

    Datatype const& datatype           () const;

    template<
        typename T>
    void           write               (T const& value);

    template<
        typename T>
    T              read                () const;

private:

    Identifier     _id;

    Datatype       _datatype;

};


Attribute          create_attribute    (Identifier const& location,
                                        std::string const& name,
                                        std::string const& value);

Attribute          create_attribute    (Identifier const& location,
                                        std::string const& name,
                                        size_t const nr_bytes);

Attribute          create_attribute    (Identifier const& location,
                                        std::string const& name,
                                        Datatype const& datatype,
                                        Dataspace const& dataspace);

} // namespace hdf5
} // namespace lue
