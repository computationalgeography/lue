#pragma once
#include "lue/hdf5/attributes.hpp"
#include <cassert>


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an open HDF5 group

    An HDF5 group can be opened multiple times.
*/
class Group
{

public:

                   Group               (Group const& parent,
                                        std::string const& name);

    explicit       Group               (Identifier const& id);

    explicit       Group               (Identifier&& id);

                   Group               (Group const&)=default;

                   Group               (Group&&)=default;

    virtual        ~Group              ()=default;

    Group&         operator=           (Group const&)=default;

    Group&         operator=           (Group&&)=default;

    bool           operator==          (Group const& other) const;

    bool           operator!=          (Group const& other) const;

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


bool               group_exists        (Group const& parent,
                                        std::string const& name);

Group              create_group        (Group const& parent,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue
