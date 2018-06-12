#pragma once
#include "lue/collection.hpp"
#include "lue/property.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_properties(hdf5::Identifier const&)
*/
class Properties:
    public Collection<Property>
{

public:

                   Properties        (hdf5::Group const& parent);

                   Properties        (Collection<Property>&& collection);

                   Properties        (Properties const&)=delete;

                   Properties        (Properties&&)=default;

                   ~Properties       ()=default;

    Properties&    operator=         (Properties const&)=delete;

    Properties&    operator=         (Properties&&)=default;

private:

};


Properties         create_properties   (hdf5::Group const& group);

} // namespace lue
