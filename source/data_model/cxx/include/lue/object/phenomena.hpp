#pragma once
#include "lue/object/phenomenon.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace data_model {

class Phenomena:
    public Collection<Phenomenon>
{

public:

    explicit       Phenomena           (hdf5::Group& parent);

    explicit       Phenomena           (Collection<Phenomenon>&& collection);

                   Phenomena           (Phenomena const&)=delete;

                   Phenomena           (Phenomena&&)=default;

                   ~Phenomena          ()=default;

    Phenomena&     operator=           (Phenomena const&)=delete;

    Phenomena&     operator=           (Phenomena&&)=default;

    Phenomenon&    add                 (std::string const& name,
                                        std::string const& description="");

private:

};


Phenomena          create_phenomena    (hdf5::Group& parent);

} // namespace data_model
} // namespace lue
