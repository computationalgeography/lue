#pragma once
#include "lue/hdf5/primary_data_object.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an open HDF5 group

    An HDF5 group can be opened multiple times.
*/
class Group:
    public PrimaryDataObject
{

public:

                   Group               (Group const& parent,
                                        std::string const& name);

    explicit       Group               (Identifier const& id);

    explicit       Group               (Identifier&& id);

                   Group               (Group const&)=default;

                   Group               (Group&&)=default;

                   ~Group              ()=default;

    Group&         operator=           (Group const&)=default;

    Group&         operator=           (Group&&)=default;

    std::vector<std::string>
                   group_names         () const;

    std::vector<std::string>
                   object_names        () const;

    bool           contains_group      (std::string const& name) const;

    bool           contains_dataset    (std::string const& name) const;

    bool           contains_soft_link  (std::string const& name) const;

    bool           contains_hard_link  (std::string const& name) const;

    void           create_soft_link    (Identifier const& location,
                                        std::string const& name);

    void           create_hard_link    (Identifier const& location,
                                        std::string const& name);

private:

};


bool               group_exists        (Group const& parent,
                                        std::string const& name);

Group              create_group        (Group const& parent,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue
