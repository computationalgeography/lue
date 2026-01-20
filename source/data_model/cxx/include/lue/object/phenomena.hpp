#pragma once
#include "lue/core/collection.hpp"
#include "lue/object/phenomenon.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT Phenomena: public Collection<Phenomenon>
    {

        public:

            explicit Phenomena(hdf5::Group const& parent);

            explicit Phenomena(Collection<Phenomenon>&& collection);

            Phenomena(Phenomena const& other) = default;

            Phenomena(Phenomena&& other) = default;

            ~Phenomena() override = default;

            auto operator=(Phenomena const& other) -> Phenomena& = default;

            auto operator=(Phenomena&& other) -> Phenomena& = default;

            auto add(std::string const& name, std::string const& description = "") -> Phenomenon&;

        private:
    };


    auto create_phenomena(hdf5::Group& parent) -> Phenomena;

}  // namespace lue::data_model
