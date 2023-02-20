#pragma once
#include <string>


namespace lue {
    namespace data_model {

        // array
        static std::string const datatype_tag{"lue_datatype"};
        static std::string const nr_objects_tag{"lue_nr_objects"};
        static std::string const rank_tag{"lue_rank"};
        // static std::string const shape_per_object_tag{"lue_shape_per_object"};
        static std::string const value_variability_tag{"lue_value_variability"};
        static std::string const shape_variability_tag{"lue_shape_variability"};

        static std::string const description_tag{"lue_description"};

        // info
        static std::string const object_tracker_tag{"lue_object_tracker"};
        static std::string const object_id_tag{"lue_object_id"};
        static std::string const active_object_id_tag{"lue_active_object_id"};
        static std::string const active_set_index_tag{"lue_active_set_index"};
        static std::string const active_object_index_tag{"lue_active_object_index"};
        static std::string const coordinates_tag{"lue_coordinates"};
        static std::string const nr_locations_in_time_tag{"lue_nr_locations_in_time"};
        static std::string const value_tag{"lue_value"};
        static std::string const epoch_kind_tag{"lue_epoch_kind"};
        static std::string const epoch_calendar_tag{"lue_epoch_calendar"};
        static std::string const epoch_origin_tag{"lue_epoch_origin"};

        // object
        static std::string const time_domain_tag{"lue_time_domain"};
        static std::string const time_domain_item_type_tag{"lue_time_domain_item_type"};
        static std::string const space_domain_tag{"lue_space_domain"};
        static std::string const space_domain_item_type_tag{"lue_space_domain_item_type"};
        static std::string const mobility_tag{"lue_mobility"};
        static std::string const properties_tag{"lue_properties"};
        // static std::string const property_set_tag{"lue_property_set"};
        static std::string const shape_per_object_tag{"lue_shape_per_object"};
        static std::string const same_shape_tag{"lue_same_shape"};
        static std::string const same_shape_constant_shape_tag{"lue_same_shape_constant_shape"};
        static std::string const same_shape_variable_shape_tag{"lue_same_shape_variable_shape"};
        static std::string const different_shape_tag{"lue_different_shape"};
        static std::string const different_shape_constant_shape_tag{"lue_different_shape_constant_shape"};
        static std::string const different_shape_variable_shape_tag{"lue_different_shape_variable_shape"};
        static std::string const phenomena_tag{"lue_phenomena"};
        static std::string const collection_property_sets_tag{"lue_collection_property_sets"};
        static std::string const property_sets_tag{"lue_property_sets"};
        static std::string const universes_tag{"lue_universes"};

        static std::string const presence_discretization_property_tag{"lue_presence_discretization_property"};

        static std::string const time_discretization_tag{"lue_time_discretization"};
        static std::string const time_discretization_property_tag{"lue_time_discretization_property"};
        static std::string const space_discretization_tag{"lue_space_discretization"};
        static std::string const space_discretization_property_tag{"lue_space_discretization_property"};

        static std::string const time_unit_tag{"lue_time_unit"};
        static std::string const nr_time_units_tag{"lue_nr_time_units"};

    }  // namespace data_model
}  // namespace lue
