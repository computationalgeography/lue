#pragma once
#include "lue/item/item_configuration.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_property(hdf5::Group&, std::string const&,
                ItemConfiguration const&)
*/
class Property:
    public hdf5::Group
{

public:

                   Property            (hdf5::Group const& parent,
                                        std::string const& name);

                   Property            (hdf5::Group&& group);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    ItemConfiguration
                   configuration       () const;

    void           discretize_space    (Property const& property);

    bool           space_is_discretized() const;

    Property       space_discretization() const;

private:

};


Property           create_property     (hdf5::Group const& group,
                                        std::string const& name);

}  // namespace lue
