#pragma once
#include "lue/cxx_api/phenomena.h"
#include <memory>
#include <string>


namespace lue {

bool               universe_exists     (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class Universe:
    public hdf5::Group
{

public:

                   Universe            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Universe            (hdf5::Identifier&& location);

                   Universe            (Universe const& other)=delete;

                   Universe            (Universe&& other)=default;

                   ~Universe           ()=default;

    Universe&      operator=           (Universe const& other)=delete;

    Universe&      operator=           (Universe&& other)=default;

    Phenomenon&    add_phenomenon      (std::string const& name);

    Phenomena&     phenomena           () const;

private:

    std::unique_ptr<Phenomena> _phenomena;

};


Universe           create_universe     (hdf5::Identifier const& location,
                                        std::string const& name);

} // namespace lue
