#pragma once
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/space.hpp"


namespace lue::data_model {

    /*!
        @brief      Aspects that determine how locations in space are represented
    */
    using SpaceConfiguration = Configuration<
        Mobility,
        SpaceDomainItemType

        >;


    /*!
        @brief      Class representing the space domain
    */
    class LUE_DATA_MODEL_EXPORT SpaceDomain: public hdf5::Group
    {

        public:

            explicit SpaceDomain(hdf5::Group const& parent);

            explicit SpaceDomain(hdf5::Group&& group);

            SpaceDomain(SpaceDomain const& other) = default;

            SpaceDomain(SpaceDomain&& other) = default;

            ~SpaceDomain() override = default;

            auto operator=(SpaceDomain const& other) -> SpaceDomain& = default;

            auto operator=(SpaceDomain&& other) -> SpaceDomain& = default;

            auto configuration() const -> SpaceConfiguration const&;

            template<typename Value>
            auto value() -> Value;

            template<typename Value>
            auto value(hdf5::Datatype const& datatype) -> Value;

            auto presence_is_discretized() const -> bool;

            void set_presence_discretization(PropertyGroup& property);

            auto discretized_presence_property() -> PropertyGroup;

        private:

            SpaceConfiguration _configuration;
    };


    auto create_space_domain(
        hdf5::Group& parent,
        SpaceConfiguration const& configuration,
        hdf5::Datatype const& datatype,
        std::size_t rank) -> SpaceDomain;

    auto space_domain_exists(hdf5::Group const& parent) -> bool;


    /*!
        @brief      Return instance representing the collection of space
                    domain object arrays
        @tparam     Value Class corresponding with the instance's configuration()
    */
    template<typename Value>
    inline auto SpaceDomain::value() -> Value
    {
        return Value{*this};
    }


    /*!
        @brief      Return instance representing the collection of space
                    domain object arrays
        @tparam     Value Class corresponding with the instance's configuration()
        @param      datatype In-memory datatype of the space domain items
    */
    template<typename Value>
    inline auto SpaceDomain::value(hdf5::Datatype const& datatype) -> Value
    {
        return Value{*this, datatype};
    }

}  // namespace lue::data_model
