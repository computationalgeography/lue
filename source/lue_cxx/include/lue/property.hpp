#pragma once
#include "lue/define.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @brief      TODO
    @sa         create_property(hdf5::Group&, std::string const&,
                Property::Configuration const&)
*/
class Property:
    public hdf5::Group
{

public:

    class Configuration
    {

    public:

                   Configuration       (ShapePerItemType const type);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const& other)=default;

                   ~Configuration      ()=default;

        Configuration& operator=       (Configuration const& other)=default;

        ShapePerItemType
                   shape_per_item_type () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        ShapePerItemType
                   _shape_per_item_type;

        void       load                (hdf5::Attributes const& attributes);

    };


                   Property            (hdf5::Group const& parent,
                                        std::string const& name);

                   Property            (hdf5::Group&& group);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    Configuration const&
                   configuration       () const;

    void           discretize_space    (Property const& property);

    bool           space_is_discretized() const;

    Property       space_discretization() const;

private:

    Configuration  _configuration;

};


Property           create_property     (hdf5::Group const& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace lue
