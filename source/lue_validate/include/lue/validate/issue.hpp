#pragma once
#include "lue/hdf5/identifier.hpp"
#include <string>
#include <vector>


namespace lue {
namespace utility {

class Issue
{

public:

                   Issue               (hdf5::Identifier const& id,
                                        std::string const& message);

                   Issue               (Issue const&)=delete;

                   Issue               (Issue&& other)=default;

                   ~Issue              ()=default;

    Issue&         operator=           (Issue const&)=delete;

    Issue&         operator=           (Issue&&)=delete;

    hdf5::Identifier const&
                   id                  () const;

    std::string const&
                   message             () const;

private:

    hdf5::Identifier _id;

    std::string    _message;

};

}  // namespace utility
}  // namespace lue
