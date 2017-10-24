#pragma once
#include "lue/constant_size/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class Domain:
    public constant_size::Domain
{

public:

    // class Configuration
    // {

    // public:

    //     enum class ItemType
    //     {

    //         box

    //     };

    //                Configuration       (ItemType const item_type);

    //                Configuration       (hdf5::Attributes const& attributes);

    //                Configuration       (Configuration const& other)=default;

    //                ~Configuration      ()=default;

    //     Configuration& operator=       (Configuration const& other)=default;

    //     ItemType   item_type           () const;

    //     void       save                (hdf5::Attributes& attributes) const;

    // private:

    //     ItemType   _item_type;

    //     void       load                (hdf5::Attributes const& attributes);

    // };


                   Domain         (hdf5::Identifier const& location);

                   Domain         (constant_size::Domain&& domain);

                   Domain         (Domain const& other)=delete;

                   Domain         (Domain&& other)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const& other)=delete;

    Domain&        operator=      (Domain&& other)=default;

    // Configuration const& configuration() const;

private:

    // Configuration  _configuration;

};

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
