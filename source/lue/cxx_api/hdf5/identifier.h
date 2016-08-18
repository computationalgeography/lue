#pragma once
#include <hdf5.h>
#include <functional>
#include <string>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
    @brief      This class represents an HDF5 identifier.

    Scoping the identifier in this class ensures that the identifier is
    closed upon exiting the scope.
*/
class Identifier
{

public:


    /*!
        @brief      Type of function to call when the identifier must
                    be closed.
    */
    using Close = std::function<void(hid_t)>;

                   Identifier          (hid_t id,
                                        Close const& close);

                   Identifier          (Identifier const& other)=delete;

                   Identifier          (Identifier&& other);

    virtual        ~Identifier         ();

    Identifier&    operator=           (Identifier const& other)=delete;

    Identifier&    operator=           (Identifier&& other);

    bool           is_valid            () const;

                   operator hid_t      () const;

    std::string    pathname            () const;

private:

    //! HDF5 identifier.
    hid_t          _id;

    //! Function to call when the identifier must be closed.
    Close          _close;

};

} // namespace hdf5
} // namespace lue
