#pragma once
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/space.hpp"


namespace lue {
    namespace data_model {

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
        class SpaceDomain: public hdf5::Group
        {

            public:

                explicit SpaceDomain(hdf5::Group const& parent);

                explicit SpaceDomain(hdf5::Group&& group);

                SpaceDomain(SpaceDomain const&) = default;

                SpaceDomain(SpaceDomain&&) = default;

                ~SpaceDomain() override = default;

                SpaceDomain& operator=(SpaceDomain const&) = default;

                SpaceDomain& operator=(SpaceDomain&&) = default;

                SpaceConfiguration const& configuration() const;

                template<typename Value>
                Value value();

                template<typename Value>
                Value value(hdf5::Datatype const& datatype);

                bool presence_is_discretized() const;

                void set_presence_discretization(PropertyGroup& property);

                PropertyGroup discretized_presence_property();

            private:

                SpaceConfiguration _configuration;
        };


        SpaceDomain create_space_domain(
            hdf5::Group& parent,
            SpaceConfiguration const& configuration,
            hdf5::Datatype const& datatype,
            std::size_t rank);

        bool space_domain_exists(hdf5::Group const& parent);


        /*!
            @brief      Return instance representing the collection of space
                        domain object arrays
            @tparam     Value Class corresponding with the instance's configuration()
        */
        template<typename Value>
        inline Value SpaceDomain::value()
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
        inline Value SpaceDomain::value(hdf5::Datatype const& datatype)
        {
            return Value{*this, datatype};
        }

    }  // namespace data_model
}  // namespace lue
