#pragma once
#include "lue/hdf5/hard_link.hpp"
#include "lue/hdf5/soft_link.hpp"
#include "lue/hdf5/primary_data_object.hpp"
#include <memory>


namespace lue {
namespace hdf5 {

// Forward declare the File class to be able to declare it a friend of
// the Group class
class File;


/*!
    @brief      This class represents an open HDF5 group

    An HDF5 group can be opened multiple times.
*/
class Group:
    public PrimaryDataObject
{

    friend File;

public:

                   Group               (Group& parent,
                                        std::string const& name);

                   Group               (Group& parent,
                                        Identifier& id);

                   Group               (Group& other);

                   Group               (Group&&)=default;

                   ~Group              ()=default;

    Group&         operator=           (Group const& other)=delete;

    Group&         operator=           (Group&&)=default;

    bool           has_parent          () const;

    Group&         parent              ();

    std::vector<std::string>
                   group_names         () const;

    std::vector<std::string>
                   object_names        () const;

    bool           contains_group      (std::string const& name) const;

    bool           contains_dataset    (std::string const& name) const;

    bool           contains_soft_link  (std::string const& name) const;

    bool           contains_hard_link  (std::string const& name) const;

    SoftLink       soft_link           (std::string const& name);

    HardLink       hard_link           (std::string const& name);

    SoftLink       create_soft_link    (Identifier const& location,
                                        std::string const& name);

    HardLink       create_hard_link    (Identifier const& location,
                                        std::string const& name);

protected:

private:

    explicit       Group               (Identifier&& id);

    //! Parent group
    std::unique_ptr<Group> _parent;

};


bool               group_exists        (Group const& parent,
                                        std::string const& name);

Group              create_group        (Group& parent,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue
