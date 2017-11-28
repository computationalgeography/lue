#pragma once
#include "lue/phenomena.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_universe(hdf5::Identifier const&, std::string const&)
*/
class Universe:
    public hdf5::Group
{

public:

                   Universe            (hdf5::Group const& parent,
                                        std::string const& name);

                   Universe            (hdf5::Group&& group);

                   Universe            (Universe const&)=delete;

                   Universe            (Universe&&)=default;

                   ~Universe           ()=default;

    Universe&      operator=           (Universe const&)=delete;

    Universe&      operator=           (Universe&&)=default;

    Phenomenon&    add_phenomenon      (std::string const& name);

    Phenomena const& phenomena         () const;

    Phenomena&     phenomena           ();

private:

    Phenomena      _phenomena;

};


Universe           create_universe     (hdf5::Group const& parent,
                                        std::string const& name);

} // namespace lue
