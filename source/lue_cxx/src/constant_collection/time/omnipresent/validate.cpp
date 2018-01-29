#include "lue/constant_collection/time/omnipresent/validate.hpp"
#include "lue/constant_collection/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_collection/time/omnipresent/same_shape/property.hpp"
#include "lue/hdf5/validate.hpp"
#include <boost/format.hpp>


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

void validate(
    PropertySet const& property_set,
    hdf5::Issues& issues)
{
    auto const nr_ids = property_set.ids().nr_items();

    // if(performance_is_not_an_issue) {
    //     hdf5::assert_unique_values(property_set.ids(), issues);
    // }

    auto const& properties = property_set.properties();

    for(auto const& name: properties.names()) {
        auto const& property = properties[name];
        auto const& configuration = property.configuration();
        size_t nr_values;

        switch(configuration.type<ShapePerItem>()) {

            case ShapePerItem::same: {
                auto file_datatype =
                    same_shape::Property::file_datatype(property);
                auto const property2 = same_shape::Property(
                    hdf5::Group{property.id()}, memory_datatype(file_datatype));
                nr_values =  property2.values().nr_items();

                break;
            }

            case ShapePerItem::different: {
                auto file_datatype =
                    different_shape::Property::file_datatype(property);
                auto const property2 = different_shape::Property(
                    hdf5::Group{property.id()}, memory_datatype(file_datatype));
                nr_values =  property2.values().nr_items();

                break;
            }

        }

        if(nr_values != nr_ids) {
            issues.add_error(
                property.id(),
                boost::str(boost::format(
                    "property %1%: nr property values (%2%) != "
                    "nr property-set ids (%3%)")
                    % name
                    % nr_values
                    % nr_ids
                )
            );
        }
    }
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
