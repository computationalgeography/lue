#pragma once
#include "lue/property_sets.hpp"


namespace lue {

// bool               phenomenon_exists   (hdf5::Identifier const& location,
//                                         std::string const& name);


/*!
    @brief      TODO
*/
class Phenomenon:
    public hdf5::Group
{

public:

                   Phenomenon          (hdf5::Identifier const& location,
                                        std::string const& name);

                   Phenomenon          (hdf5::Identifier&& location);

                   Phenomenon          (hdf5::Group&& group);

                   Phenomenon          (Phenomenon const& other)=delete;

                   Phenomenon          (Phenomenon&& other)=default;

                   ~Phenomenon         ()=default;

    Phenomenon&    operator=           (Phenomenon const& other)=delete;

    Phenomenon&    operator=           (Phenomenon&& other)=default;

    // PropertySet&   add_property_set    (std::string const& name);

    // PropertySet&   add_property_set    (std::string const& name,
    //                                     PropertySetConfiguration const&
    //                                         configuration,
    //                                     DomainConfiguration const&
    //                                         domain_configuration);

    PropertySets const& property_sets  () const;

    PropertySets&  property_sets       ();

private:

    PropertySets   _property_sets;

};


Phenomenon         create_phenomenon   (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace lue
