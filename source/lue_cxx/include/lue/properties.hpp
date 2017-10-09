#pragma once
#include "lue/collection.hpp"
#include "lue/property.hpp"


namespace lue {

/*!
    @brief      TODO
*/
class Properties:
    public Collection<Property>
{

public:

                   Properties        (hdf5::Identifier const& location);

                   Properties        (Collection<Property>&& collection);

                   Properties        (Properties const& other)=delete;

                   Properties        (Properties&& other)=default;

                   ~Properties       ()=default;

    Properties&    operator=         (Properties const& other)=delete;

    Properties&    operator=         (Properties&& other)=default;

private:

};


Properties         create_properties   (hdf5::Identifier const& location);

} // namespace lue
