#pragma once
#include "lue/hdf5/identifier.hpp"
#include <string>
#include <vector>


namespace lue {
namespace utility {

/*!
    @brief      Information about issue found during validation
*/
class Issue
{

public:

                   Issue               (hdf5::Identifier const& id,
                                        std::string const& message);

                   Issue               (Issue const&)=default;

                   Issue               (Issue&& other)=default;

                   ~Issue              ()=default;

    Issue&         operator=           (Issue const&)=default;

    Issue&         operator=           (Issue&&)=default;

    hdf5::Identifier const&
                   id                  () const;

    std::string const&
                   message             () const;

private:

    //! Id of HDF5 object related to the issue
    hdf5::Identifier _id;

    //! Message describing the issue
    std::string    _message;

};

}  // namespace utility
}  // namespace lue
