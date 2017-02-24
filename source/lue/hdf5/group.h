#pragma once
#include "lue/hdf5/attributes.h"
#include "lue/hdf5/dataset.h"
#include <cassert>


namespace lue {
namespace hdf5 {

bool               group_exists        (Identifier const& location,
                                        std::string const& name);


/*!
    @brief      This class represents an HDF5 group.
*/
class Group
{

public:

                   Group               (Identifier const& location,
                                        std::string const& name);

                   Group               (Identifier const& id);

                   Group               (Identifier&& id);

                   Group               (Group const& other)=delete;

                   Group               (Group&& other);

    virtual        ~Group              ()=default;

    Group&         operator=           (Group const& other)=delete;

    Group&         operator=           (Group&& other);

    Identifier const& id               () const;

    Attributes const& attributes       () const;

    Attributes&    attributes          ();

    template<
        typename T>
    T              attribute           (std::string const& name) const;

    std::vector<std::string>
                   group_names         () const;

    bool           contains_group      (std::string const& name) const;

    bool           contains_dataset    (std::string const& name) const;

    bool           contains_attribute  (std::string const& name) const;

    bool           contains_soft_link  (std::string const& name) const;

    void           create_soft_link    (Identifier const& location,
                                        std::string const& name);

private:

    Identifier     _id;

    Attributes     _attributes;

};


template<
    typename T>
inline T Group::attribute(
    std::string const& name) const
{
    assert(contains_attribute(name));

    return _attributes.read<T>(name);
}


bool               operator==          (Group const& lhs,
                                        Group const& rhs);

bool               operator!=          (Group const& lhs,
                                        Group const& rhs);

// Group              open_group          (Group const& group,
//                                         std::string const& name);
// 
// Group              open_group          (Identifier const& id,
//                                         std::string const& name);

Group              create_group        (Identifier const& id,
                                        std::string const& name);

std::vector<std::string>
                   object_names        (Group const& group);

// std::vector<std::string>
//                    group_names         (Group const& group);

std::vector<std::string>
                   dataset_names       (Group const& group);

// Dataset            open_dataset        (Group const& group,
//                                         std::string const& name);

} // namespace hdf5
} // namespace lue
