#pragma once
#include "lue/hdf5/identifier.hpp"
#include <string>
#include <vector>


namespace lue {
namespace hdf5 {

/*!
    @brief      Information about issue found during validation
*/
class Issue
{

public:

                   Issue               (Identifier const& id,
                                        std::string const& message);

                   Issue               (Issue const&)=default;

                   Issue               (Issue&&)=default;

                   ~Issue              ()=default;

    Issue&         operator=           (Issue const&)=default;

    Issue&         operator=           (Issue&&)=default;

    Identifier const& id               () const;

    std::string const&
                   message             () const;

private:

    //! Id of HDF5 object related to the issue
    Identifier     _id;

    //! Message describing the issue
    std::string    _message;

};

}  // namespace hdf5
}  // namespace lue
