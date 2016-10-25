#pragma once
#include "lue/utility/lue_validate/error.h"
#include "lue/utility/lue_utility/application.h"
#include "lue/cxx_api/define.h"
#include "lue/cxx_api/hdf5.h"
#include "lue/cxx_api/shape.h"
#include <vector>
#include <string>
#include <tuple>


namespace lue {

class Validate:
    public Application

{

public:

                   Validate            (int argc,
                                        char* argv[],
                                        std::string const& usage);

    void           run_implementation  () override;

private:

    mutable std::vector<Error> _errors;

    // void           print_message       (hdf5::Identifier const& id,
    //                                     std::string const& message) const;

    // void           print_error         (Error const& error) const;

    void           print_errors        () const;

    void           not_implemented     (std::string const& message) const;

    void           missing_group       (hdf5::Identifier const& id,
                                        std::string const& name) const;

    void           missing_dataset     (hdf5::Identifier const& id,
                                        std::string const& name) const;

    void           missing_attribute   (hdf5::Identifier const& id,
                                        std::string const& name) const;

    void           missing_soft_link   (hdf5::Identifier const& id,
                                        std::string const& name) const;

    void           link_does_not_point_to_group(
                                        hdf5::Group const& group,
                                        std::string const& name,
                                        hdf5::Group const& target) const;

    void           invalid_mobility    (hdf5::Group const& group,
                                        SpaceDomainType const domain_type,
                                        Mobility const mobility) const;

    void           invalid_domain_item_type(
                                        hdf5::Group const& group,
                                        SpaceDomainType const domain_type,
                                        SpaceDomainItemType const
                                            domain_item_type) const;

    void           invalid_domain_item_type(
                                        hdf5::Group const& group,
                                        TimeDomainType const domain_type,
                                        TimeDomainItemType const
                                            domain_item_type) const;

    void           assert_groups_exist (hdf5::Group const& group,
                                        std::vector<std::string> const& names)
                                            const;

    void           assert_datasets_exist(
                                        hdf5::Group const& group,
                                        std::vector<std::string> const& names)
                                            const;

    void           assert_shape_equals (hdf5::Dataset const& dataset,
                                        hdf5::Dataspace const& dataspace,
                                        Shape const& shape) const;

    void           assert_shape_equals (hdf5::Dataset const& dataset,
                                        Shape const& shape) const;

    void           assert_rank_equals  (hdf5::Dataset const& dataset,
                                        hdf5::Dataspace const& dataspace,
                                        size_t const rank) const;

    void           assert_rank_equals  (hdf5::Dataset const& dataset,
                                        size_t const rank) const;

    void           assert_dataspace_equals(
                                        hdf5::Dataset const& dataset,
                                        Shape const& shape) const;

    void           assert_datatype_equals(
                                        hdf5::Dataset const& dataset,
                                        hid_t const datatype) const;

    void           assert_attributes_exist(
                                        hdf5::Group const& group,
                                        std::vector<std::string> const& names)
                                            const;

    void           assert_soft_links_exist(
                                        hdf5::Group const& group,
                                        std::vector<std::string> const& names)
                                            const;

    void           assert_link_points_to(
                                        hdf5::Group const& group,
                                        std::string const& name,
                                        hdf5::Group const& target) const;

    // void           validate_property   (hdf5::Group const& group) const;

    // void           validate_space_domain(
    //                                     hdf5::Group const& group,
    //                                     SpaceDomainType const domain_type,
    //                                     Mobility const mobility,
    //                                     SpaceDomainItemType const
    //                                         domain_item_type) const;

    // void           validate_space_domain(
    //                                     hdf5::Group const& group) const;

    // void           validate_domains    (hdf5::Group const& group) const;

    // void           validate_sc_periods (hdf5::Group const& group) const;

    // void           validate_time_domain(hdf5::Group const& group,
    //                                     TimeDomainType const domain_type,
    //                                     TimeDomainItemType const
    //                                         domain_item_type) const;

    // void           validate_time_domain(hdf5::Group const& group) const;

    // void           validate_domain     (hdf5::Group const& group) const;

    void           validate_cs_value   (hdf5::Group const& domain,
                                        hdf5::Group const& property) const;

    void           validate_vs_value   (hdf5::Group const& domain,
                                        hdf5::Group const& property) const;

    void           o_none_validate_properties(
                                        hdf5::Group const& domain,
                                        hdf5::Group const& group) const;

    void           o_validate_property (hdf5::Group const& group) const;

    void           o_box_validate_properties(
                                        hdf5::Group const& domain,
                                        hdf5::Group const& group) const;

    void           sc_p_s_box_validate_properties(
                                        hdf5::Group const& domain,
                                        hdf5::Group const& group) const;

    void           o_validate_properties(
                                        hdf5::Group const& domain,
                                        hdf5::Group const& group,
                                        SpaceDomainType const
                                            space_domain_type,
                                        Mobility const mobility,
                                        SpaceDomainItemType const
                                            space_domain_item_type) const;

    void           sc_p_validate_properties(
                                        hdf5::Group const& domain,
                                        hdf5::Group const& group,
                                        SpaceDomainType const
                                            space_domain_type,
                                        Mobility const mobility,
                                        SpaceDomainItemType const
                                            space_domain_item_type) const;

    void           validate_properties (hdf5::Group const& domain,
                                        hdf5::Group const& group,
                                        TimeDomainType const time_domain_type,
                                        TimeDomainItemType const
                                            time_domain_item_type,
                                        SpaceDomainType const
                                            space_domain_type,
                                        Mobility const mobility,
                                        SpaceDomainItemType const
                                            space_domain_item_type) const;

    void           validate_properties (hdf5::Group const& domain,
                                        hdf5::Group const& group) const;

    void           validate_property_set(
                                        hdf5::Group const& group) const;

    void           validate_property_sets(
                                        hdf5::Group const& group) const;

    void           validate_phenomenon (hdf5::Group const& group) const;

    void           validate_phenomena  (hdf5::Group const& group) const;

    void           validate_universe   (hdf5::Group const& group) const;

    void           validate_universes  (hdf5::Group const& group) const;

    void           validate_dataset    (std::string const& filename) const;

};

}  // namespace lue
