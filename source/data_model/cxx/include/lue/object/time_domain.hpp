#pragma once
#include "lue/core/clock.hpp"
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"
#include "lue/info/time.hpp"


namespace lue::data_model {

    using TimeConfiguration = Configuration<TimeDomainItemType>;


    class LUE_DATA_MODEL_EXPORT TimeDomain: public hdf5::Group
    {

        public:

            explicit TimeDomain(hdf5::Group const& parent);

            explicit TimeDomain(hdf5::Group&& group);

            TimeDomain(TimeDomain const& other) = default;

            TimeDomain(TimeDomain&& other) = default;

            ~TimeDomain() override = default;

            auto operator=(TimeDomain const& other) -> TimeDomain& = default;

            auto operator=(TimeDomain&& other) -> TimeDomain& = default;

            auto configuration() const -> TimeConfiguration const&;

            auto clock() const -> Clock const&;

            template<typename T>
            auto value() -> T;

        private:

            TimeConfiguration _configuration;

            Clock _clock;
    };


    auto create_time_domain(
        hdf5::Group& parent, TimeConfiguration const& configuration, data_model::Clock const& clock)
        -> TimeDomain;

    void link_time_domain(hdf5::Group& parent, TimeDomain& domain);

    auto has_linked_time_domain(hdf5::Group const& parent) -> bool;

    auto time_domain_exists(hdf5::Group const& parent) -> bool;


    template<typename T>
    inline auto TimeDomain::value() -> T
    {
        return T{*this};
    }

}  // namespace lue::data_model
