#pragma once
#include "lue/cxx_api/hdf5.h"
#include <functional>
#include <string>
#include <vector>


namespace lue {

class Error
{

public:

                   Error               (hdf5::Identifier const& id,
                                        std::string const& message);

                   Error               (Error const&)=delete;

                   Error               (Error&& other)=default;

                   ~Error              ()=default;

    Error&         operator=           (Error const&)=delete;

    Error&         operator=           (Error&&)=delete;

    hdf5::Identifier const&
                   id                  () const;

    std::string const&
                   message             () const;

private:

    hdf5::Identifier _id;

    std::string    _message;

};


using Errors = std::vector<Error>;

}  // namespace lue
