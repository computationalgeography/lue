#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace hdf5 {

void               assert_no_superfluous_objects_exist(
                                        Group const& group,
                                        std::vector<std::string> const& names,
                                        Issues& issues);

bool               assert_groups_exist (Group const& group,
                                        std::vector<std::string> const& names,
                                        Issues& issues);

void               assert_unique_values(Dataset const& dataset,
                                        Issues& issues);

}  // namespace hdf5
}  // namespace lue
