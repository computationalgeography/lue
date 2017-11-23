#pragma once
#include "lue/hdf5/object_info.hpp"
#include <functional>
#include <memory>
#include <string>


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an HDF5 identifier
    @sa         https://support.hdfgroup.org/HDF5/doc/RM/RM_H5I.html

    Scoping the identifier in this class ensures that the identifier is
    closed upon exiting the scope.

    Copies can be made. Only when the last copy goes out of scope will the
    identifier be closed.
*/
class Identifier
{

public:


    /*!
        @brief      Type of function to call when the identifier must
                    be closed

        For example, when an HDF5 group is opened, you would pass H5Gclose
        as the close function into the constructor.
    */
    using Close = std::function<herr_t (hid_t)>;

                   Identifier          ();

                   Identifier          (hid_t id,
                                        Close const& close);

                   Identifier          (Identifier const&)=default;

                   Identifier          (Identifier&&)=default;

                   ~Identifier         ();

    Identifier&    operator=           (Identifier const&)=default;

    Identifier&    operator=           (Identifier&&)=default;

    bool           is_valid            () const;

                   operator hid_t      () const;

    std::string    pathname            () const;

    std::string    name                () const;

    ObjectInfo     info                () const;

private:

    void           close_if_necessary  ();

    bool           is_empty            () const;

    void           assert_invariant    () const;

    //! HDF5 identifier
    std::shared_ptr<hid_t> _id;

    //! Function to call when the identifier must be closed
    Close          _close;

};


bool               operator==          (Identifier const& lhs,
                                        Identifier const& rhs);

bool               operator!=          (Identifier const& lhs,
                                        Identifier const& rhs);

} // namespace hdf5
} // namespace lue
