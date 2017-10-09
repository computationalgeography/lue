#pragma once
#include "lue/phenomena.hpp"


namespace lue {

// bool               universe_exists     (hdf5::Identifier const& location,
//                                         std::string const& name);


/*!
    @brief      TODO
    @sa         create_universe(hdf5::Identifier const&, std::string const&)
*/
class Universe:
    public hdf5::Group
{

public:

                   Universe            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Universe            (hdf5::Identifier&& location);

                   Universe            (hdf5::Group&& group);

                   Universe            (Universe const& other)=delete;

                   Universe            (Universe&& other)=default;

                   ~Universe           ()=default;

    Universe&      operator=           (Universe const& other)=delete;

    Universe&      operator=           (Universe&& other)=default;

    Phenomenon&    add_phenomenon      (std::string const& name);

    Phenomena const& phenomena         () const;

    Phenomena&     phenomena           ();

private:

    Phenomena      _phenomena;

};


Universe           create_universe     (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace lue
