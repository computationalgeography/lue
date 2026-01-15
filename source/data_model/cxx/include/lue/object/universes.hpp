#pragma once
#include "lue/core/collection.hpp"
#include "lue/object/universe.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT Universes: public Collection<Universe>
    {

        public:

            explicit Universes(hdf5::Group const& parent);

            explicit Universes(Collection<Universe>&& collection);

            Universes(Universes const& other) = default;

            Universes(Universes&& other) = default;

            ~Universes() override = default;

            auto operator=(Universes const& other) -> Universes& = default;

            auto operator=(Universes&& other) -> Universes& = default;

            auto add(std::string const& name) -> Universe&;

        private:
    };


    auto create_universes(hdf5::Group& parent) -> Universes;

}  // namespace lue::data_model
