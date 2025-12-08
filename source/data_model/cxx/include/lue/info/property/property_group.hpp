#pragma once
#include "lue/data_model/export.hpp"
#include "lue/core/define.hpp"
#include "lue/hdf5.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT PropertyGroup: public hdf5::Group
    {

        public:

            PropertyGroup(hdf5::Group const& parent, std::string const& name);

            explicit PropertyGroup(hdf5::Group&& group);

            PropertyGroup(PropertyGroup const& other) = default;

            PropertyGroup(PropertyGroup&& other) = default;

            ~PropertyGroup() override = default;

            auto operator=(PropertyGroup const& other) -> PropertyGroup& = default;

            auto operator=(PropertyGroup&& other) -> PropertyGroup& = default;

            auto name() const -> std::string;

            auto description() const -> std::string const&;

            auto time_is_discretized() const -> bool;

            auto space_is_discretized() const -> bool;

            auto time_discretization_type() const -> TimeDiscretization;

            auto space_discretization_type() const -> SpaceDiscretization;

            void set_time_discretization(TimeDiscretization type, PropertyGroup& property);

            void set_space_discretization(SpaceDiscretization type, PropertyGroup& property);

            auto time_discretization_property() -> PropertyGroup;

            auto space_discretization_property() -> PropertyGroup;

        private:

            std::string _description;
    };


    auto create_property_group(hdf5::Group& parent, std::string const& name, std::string const& description)
        -> PropertyGroup;

}  // namespace lue::data_model
