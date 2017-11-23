#pragma once
#include "lue/hdf5/attributes.hpp"
#include "lue/hdf5/dataset.hpp"
#include <cassert>


namespace lue {
namespace hdf5 {

bool               group_exists        (Identifier const& location,
                                        std::string const& name);


/*!
    @brief      This class represents an HDF5 group
*/
class Group
{

public:

                   Group               (Identifier const& location,
                                        std::string const& name);

    explicit       Group               (Identifier const& id);

                   Group               (Identifier&& id);

                   Group               (Group const&)=default;

                   Group               (Group&&)=default;

    virtual        ~Group              ()=default;

    Group&         operator=           (Group const&)=default;

    Group&         operator=           (Group&&)=default;

    Identifier const& id               () const;

    Attributes const& attributes       () const;

    Attributes&    attributes          ();

    template<
        typename T>
    T              attribute           (std::string const& name) const;

    std::vector<std::string>
                   group_names         () const;

    std::vector<std::string>
                   object_names        () const;

    bool           contains_group      (std::string const& name) const;

    bool           contains_dataset    (std::string const& name) const;

    bool           contains_attribute  (std::string const& name) const;

    bool           contains_soft_link  (std::string const& name) const;

    bool           contains_hard_link  (std::string const& name) const;

    void           create_soft_link    (Identifier const& location,
                                        std::string const& name);

    void           create_hard_link    (Identifier const& location,
                                        std::string const& name);

private:

    //! Identifier
    Identifier     _id;

    //! Attributes
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

Group              create_group        (Identifier const& location,
                                        std::string const& name);

Group              create_group        (Group const& group,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue
