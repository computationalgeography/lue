#pragma once
#include "lue/object/phenomena.hpp"


namespace lue {
namespace data_model {

class Universe:
    public hdf5::Group
{

public:

                   Universe            (hdf5::Group& parent,
                                        std::string const& name);

                   Universe            (hdf5::Group&& group);

                   Universe            (Universe const&)=delete;

                   Universe            (Universe&&)=default;

                   ~Universe           ()=default;

    Universe&      operator=           (Universe const&)=delete;

    Universe&      operator=           (Universe&&)=default;

    Phenomena const& phenomena         () const;

    Phenomena&     phenomena           ();

    Phenomenon&    add_phenomenon      (std::string const& name);

private:

    Phenomena      _phenomena;

};


Universe           create_universe     (hdf5::Group& parent,
                                        std::string const& name);

} // namespace data_model
} // namespace lue
