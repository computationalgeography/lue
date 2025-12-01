#pragma once
#include "lue/hdf5/issues.hpp"


// TODO: This module is not needed anymore, I think. Remove it if so.
//       Move issue/issues modules to lue_cxx, where they belong.

namespace lue::hdf5 {

    class Dataset;
    class Group;


    void assert_no_superfluous_objects_exist(
        Group const& group, std::vector<std::string> const& names, Issues& issues);

    auto assert_groups_exist(Group const& group, std::vector<std::string> const& names, Issues& issues)
        -> bool;

    void assert_unique_values(Dataset const& dataset, Issues& issues);

}  // namespace lue::hdf5
