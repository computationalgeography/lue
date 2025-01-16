#include "lue/object/universe.hpp"


namespace lue {
    namespace data_model {

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


        Phenomena const& Universe::phenomena() const
        {
            return _phenomena;
        }


        Phenomena& Universe::phenomena()
        {
            return _phenomena;
        }


        Phenomenon& Universe::add_phenomenon(std::string const& name, std::string const& description)
        {
            return _phenomena.add(name, description);
        }


        Universe create_universe(hdf5::Group& parent, std::string const& name)
        {
            hdf5::Group group{hdf5::create_group(parent, name)};

            create_phenomena(group);

            return Universe{std::move(group)};
        }

    }  // namespace data_model
}  // namespace lue
