#pragma once
#include "lue/clock.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

class TimeDomain:
    public hdf5::Group
{

public:

    class Configuration
    {

    public:

        enum class Ownership
        {

            //! Time domain items are shared between items
            shared,

            // //! Each item has a time domain item
            // unique

        };

        enum class ItemType
        {

            //! Extent in time (duration)
            box

        };

                   Configuration       (Clock const& clock,
                                        Ownership const ownership,
                                        ItemType const item_type);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const& other)=default;

                   ~Configuration      ()=default;

        Configuration& operator=       (Configuration const& other)=default;

        Clock const& clock             () const;

        Ownership  ownership           () const;

        ItemType   item_type           () const;

        void       save                (hdf5::Attributes& attributes) const;

    private:

        Clock      _clock;

        Ownership  _ownership;

        ItemType   _item_type;

        void       load                (hdf5::Attributes const& attributes);

    };


                   TimeDomain          (hdf5::Identifier const& id);

                   TimeDomain          (hdf5::Group&& group);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

    Configuration const&
                   configuration       () const;

private:

    Configuration  _configuration;

};


TimeDomain         create_time_domain  (hdf5::Group const& group,
                                        TimeDomain::Configuration const&
                                            configuration);

bool               time_domain_exists (hdf5::Group const& group);

}  // namespace lue
