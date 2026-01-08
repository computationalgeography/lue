#include "lue/object/universe.hpp"


namespace lue::data_model {

    Universe::Universe(hdf5::Group const& parent, std::string const& name):

        hdf5::Group{parent, name},
        _phenomena{*this}

    {
    }


    Universe::Universe(hdf5::Group&& group):

        hdf5::Group{std::move(group)},
        _phenomena{*this}

    {
    }


    auto Universe::phenomena() const -> Phenomena const&
    {
        return _phenomena;
    }


    auto Universe::phenomena() -> Phenomena&
    {
        return _phenomena;
    }


    auto Universe::add_phenomenon(std::string const& name, std::string const& description) -> Phenomenon&
    {
        return _phenomena.add(name, description);
    }


    auto create_universe(hdf5::Group& parent, std::string const& name) -> Universe
    {
        hdf5::Group group{hdf5::create_group(parent, name)};

        create_phenomena(group);

        return Universe{std::move(group)};
    }

}  // namespace lue::data_model
