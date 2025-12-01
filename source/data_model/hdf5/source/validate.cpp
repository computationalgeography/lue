#include "lue/hdf5/validate.hpp"
#include "lue/hdf5/group.hpp"
#include <algorithm>
#include <iterator>


namespace lue::hdf5 {
    namespace {

        void missing_group(Identifier const& id, std::string const& name, Issues& issues)
        {
            issues.add_error(id, "group '" + name + "' does not exist");
        }


        void superflouos_object(Identifier const& id, std::string const& name, Issues& issues)
        {
            issues.add_error(id, "object '" + name + "' is superfluous");
        }


        template<typename Collection>
        inline auto difference(Collection collection1, Collection collection2) -> Collection
        {
            std::sort(collection1.begin(), collection1.end());
            std::sort(collection2.begin(), collection2.end());

            Collection result;

            std::set_difference(
                collection1.begin(),
                collection1.end(),
                collection2.begin(),
                collection2.end(),
                std::inserter(result, result.begin()));

            return result;
        }

    }  // Anonymous namespace


    void assert_no_superfluous_objects_exist(
        Group const& group, std::vector<std::string> const& names, Issues& issues)
    {
        auto const superflouos_objects_names = difference(group.object_names(), names);

        // These objects are not expected.
        for (auto const& name : superflouos_objects_names)
        {
            superflouos_object(group.id(), name, issues);
        }
    }


    auto assert_groups_exist(Group const& group, std::vector<std::string> const& names, Issues& issues)
        -> bool
    {
        auto const missing_groups_names = difference(names, group.group_names());

        // These required groups are not found.
        for (auto const& name : missing_groups_names)
        {
            missing_group(group.id(), name, issues);
        }

        return missing_groups_names.empty();
    }


    void assert_unique_values([[maybe_unused]] Dataset const& dataset, [[maybe_unused]] Issues& issues)
    {
        // TODO:
        // - Switch on datatype
        // - Create buffer
        // - Read values
        // - Determine whether they are unique
    }

}  // namespace lue::hdf5
