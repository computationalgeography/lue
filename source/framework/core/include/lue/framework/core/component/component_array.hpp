#pragma once
#include "lue/framework/core/array.hpp"
#include <hpx/modules/naming_base.hpp>


namespace lue {

    template<Rank rank>
    using Localities = Array<hpx::id_type, rank>;


    /*!
        @brief      Class for n-dimensional arrays of HPX component clients
        @tparam     ComponentClient HPX Component client class
        @tparam     rank Rank of array

        Each element in the array is an instance of a HPX component
        client. These instances are related to component server instances
        which are distributed over localities.

        ComponentArray instances store the component client instances
        and the localities each server instance is located on.
    */
    template<typename ComponentClient, Rank rank>
    class ComponentArray
    {

        public:

            using Client = ComponentClient;

            using Server = typename Client::Server;

            using Clients = Array<Client, rank>;

            using Shape = typename Clients::Shape;

            using Iterator = typename Clients::Iterator;

            using ConstIterator = typename Clients::ConstIterator;


            ComponentArray():

                _localities{},
                _clients{}

            {
                assert_invariants();
            }


            ComponentArray(ComponentArray const& other) = delete;

            ComponentArray(ComponentArray&& other) = default;


            ComponentArray(Localities<rank> const& localities, Clients&& components):

                _localities{localities},
                _clients{std::move(components)}

            {
                assert_invariants();
            }


            ~ComponentArray() = default;

            ComponentArray& operator=(ComponentArray const& other) = delete;

            ComponentArray& operator=(ComponentArray&& other) = default;


            Localities<rank> const& localities() const
            {
                return _localities;
            }


            Count nr_components() const
            {
                return _clients.nr_elements();
            }


            Clients& components()
            {
                return _clients;
            }


            Clients const& components() const
            {
                return _clients;
            }


        private:

            //! Localities the partitions are located in
            Localities<rank> _localities;

            //! Array of partitions
            Clients _clients;


            void assert_invariants() const
            {
                lue_hpx_assert(_clients.shape() == _localities.shape());

                // The array is either empty, or all localities are valid / known
                lue_hpx_assert(
                    _localities.empty() ||
                    std::all_of(
                        _localities.begin(),
                        _localities.end(),

                        [](hpx::id_type const locality_id) { return bool{locality_id}; }

                        ));
            }
    };


    namespace detail {

        template<typename C, Rank r>
        class ArrayTraits<ComponentArray<C, r>>
        {

            public:

                using Element = typename ComponentArray<C, r>::Client;

                using Elements = typename ComponentArray<C, r>::Clients;

                constexpr static Rank rank = r;

                using Shape = typename ComponentArray<C, r>::Shape;

                template<typename C_, Rank r_>
                using Component = typename ComponentArray<C_, r_>::Client;

                template<typename C_, Rank r_>
                using Components = typename ComponentArray<C_, r_>::Clients;
        };

    }  // namespace detail
}  // namespace lue
