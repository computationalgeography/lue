#pragma once
#include "lue/core/collection.hpp"
#include "lue/object/universe.hpp"


namespace lue {
    namespace data_model {

        class Universes: public Collection<Universe>
        {

            public:

                explicit Universes(hdf5::Group const& parent);

                explicit Universes(Collection<Universe>&& collection);

                Universes(Universes const&) = default;

                Universes(Universes&&) = default;

                ~Universes() override = default;

                Universes& operator=(Universes const&) = default;

                Universes& operator=(Universes&&) = default;

                Universe& add(std::string const& name);

            private:
        };


        Universes create_universes(hdf5::Group& parent);

    }  // namespace data_model
}  // namespace lue
