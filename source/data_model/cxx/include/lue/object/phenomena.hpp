#pragma once
#include "lue/core/collection.hpp"
#include "lue/object/phenomenon.hpp"


namespace lue {
    namespace data_model {

        class Phenomena: public Collection<Phenomenon>
        {

            public:

                explicit Phenomena(hdf5::Group const& parent);

                explicit Phenomena(Collection<Phenomenon>&& collection);

                Phenomena(Phenomena const&) = default;

                Phenomena(Phenomena&&) = default;

                ~Phenomena() override = default;

                Phenomena& operator=(Phenomena const&) = default;

                Phenomena& operator=(Phenomena&&) = default;

                Phenomenon& add(std::string const& name, std::string const& description = "");

            private:
        };


        Phenomena create_phenomena(hdf5::Group& parent);

    }  // namespace data_model
}  // namespace lue
