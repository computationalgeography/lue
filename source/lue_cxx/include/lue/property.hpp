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


                   Property            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Property            (hdf5::Group&& group);

                   Property            (Property const& other)=default;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=default;

    Property&      operator=           (Property&& other)=default;

    Configuration const&
                   configuration       () const;

    void           discretize_time     (Property const& property);

    void           discretize_space    (Property const& property);

    bool           time_is_discretized () const;

    bool           space_is_discretized() const;

    Property       time_discretization () const;

    Property       space_discretization() const;

private:

    Configuration  _configuration;

};


Property           create_property     (hdf5::Group& group,
                                        std::string const& name,
                                        Property::Configuration const&
                                            configuration);
                                        // hdf5::Datatype const& file_datatype,
                                        // hdf5::Datatype const& memory_datatype);

}  // namespace lue
