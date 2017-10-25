#include "lue/constant_size/time/omnipresent/validate.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/hdf5/validate.hpp"
#include <boost/format.hpp>


namespace lue {
namespace constant_size {
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

        switch(configuration.shape_per_item_type()) {

            case ShapePerItemType::same: {
                auto file_datatype = same_shape::Property::file_datatype(
                    property.id());
                auto const property2 = same_shape::Property(
                    property, memory_datatype(file_datatype));
                nr_values =  property2.values().nr_items();

                break;
            }

            case ShapePerItemType::different: {
                auto file_datatype = different_shape::Property::file_datatype(
                    property.id());
                auto const property2 = different_shape::Property(
                    property, memory_datatype(file_datatype));
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
}  // namespace constant_size
}  // namespace lue
