#pragma once
#include "lue/define.h"
#include "lue/hdf5/group.h"


namespace lue {

class Property:
    public hdf5::Group
{

public:

    class Configuration
    {

    public:

                   Configuration       (ShapePerItemType const type);

                   Configuration       (hdf5::Attributes const& attributes);

                   ~Configuration()=default;

        ShapePerItemType
                   shape_per_item_type () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        ShapePerItemType
                   _shape_per_item_type;

        void       load                (hdf5::Attributes const& attributes);

    };

                   Property            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Property            (hdf5::Group&& group);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    Configuration const&
                   configuration       () const;

private:

    Configuration  _configuration;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);

}  // namespace lue
