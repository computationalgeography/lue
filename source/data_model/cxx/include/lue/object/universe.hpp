#pragma once
#include "lue/object/phenomena.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT Universe: public hdf5::Group
    {

        public:

            Universe(hdf5::Group const& parent, std::string const& name);

            explicit Universe(hdf5::Group&& group);

            Universe(Universe const& other) = default;

            Universe(Universe&& other) = default;

            ~Universe() override = default;

            auto operator=(Universe const& other) -> Universe& = default;

            auto operator=(Universe&& other) -> Universe& = default;

            auto phenomena() const -> Phenomena const&;

            auto phenomena() -> Phenomena&;

            auto add_phenomenon(std::string const& name, std::string const& description = "") -> Phenomenon&;

        private:

            Phenomena _phenomena;
    };


    auto create_universe(hdf5::Group& parent, std::string const& name) -> Universe;

}  // namespace lue::data_model
