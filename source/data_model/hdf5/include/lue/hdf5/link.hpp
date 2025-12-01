#pragma once
#include "lue/hdf5/identifier.hpp"


namespace lue::hdf5 {

    class Group;


    class Link
    {

        public:

            Link(Group const& group, std::string name);

            Link(Link const& other) = default;

            Link(Link&& other) = default;

            ~Link() = default;

            auto operator=(Link const& other) -> Link& = default;

            auto operator=(Link&& other) -> Link& = default;

            auto location_id() const -> Identifier const&;

            auto name() const -> std::string const&;

        protected:

            auto value_size() const -> std::size_t;

        private:

            auto info() const -> H5L_info_t;

            Identifier _location_id;

            std::string _name;
    };


    auto object_type(Identifier const& id, std::string const& name) -> H5O_type_t;

    auto link_info(Identifier const& id, std::string const& name) -> H5L_info_t;

    auto link_type(Identifier const& id, std::string const& name) -> H5L_type_t;

    auto link_value_size(Identifier const& id, std::string const& name) -> std::size_t;

    auto link_exists(Identifier const& id, std::string const& name) -> bool;

    auto hard_link_exists(Identifier const& id, std::string const& name) -> bool;

    auto link_is_dataset(Identifier const& id, std::string const& name) -> bool;

    auto link_is_group(Identifier const& id, std::string const& name) -> bool;

}  // namespace lue::hdf5
