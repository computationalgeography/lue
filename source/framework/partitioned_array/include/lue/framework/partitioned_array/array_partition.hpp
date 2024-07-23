#pragma once
#include "lue/framework/partitioned_array/server/array_partition.hpp"


namespace lue {

    /*!
        @brief      Class for array partition component client instances
        @tparam     Element Type for representing element values
        @tparam     rank Array rank

        Each instance is associated with a single array partition component server instance. Multiple
        instances can be associated with a single server instance.

        A partition has an offset, which represents the location of the partition within the array. An offset
        contains an index along each dimension of the array. For example, the first partition in a 2D array is
        located at offset [0, 0].
    */
    template<typename Element, Rank rank>
    class LUE_FPA_EXPORT ArrayPartition:
        public hpx::components::
            client_base<ArrayPartition<Element, rank>, server::ArrayPartition<Element, rank>>

    {

        public:

            using Server = server::ArrayPartition<Element, rank>;

            using Base = hpx::components::client_base<ArrayPartition<Element, rank>, Server>;

            using Data = typename Server::Data;

            using Offset = typename Server::Offset;

            using Shape = typename Server::Shape;

            using Slice = typename Server::Slice;

            using Slices = typename Server::Slices;

            ArrayPartition();

            explicit ArrayPartition(hpx::id_type const& component_id);

            explicit ArrayPartition(hpx::future<hpx::id_type>&& component_id);

            ArrayPartition(hpx::future<ArrayPartition>&& partition);

            ArrayPartition(hpx::id_type locality_id, Offset const& offset, Shape const& shape);

            ArrayPartition(hpx::id_type locality_id, Offset const& offset, Shape const& shape, Element value);

            ArrayPartition(hpx::id_type locality_id, Offset const& offset, Data&& data);

            ArrayPartition(ArrayPartition const&) = default;

            ArrayPartition(ArrayPartition&&) = default;

            ~ArrayPartition() = default;

            ArrayPartition& operator=(ArrayPartition const&) = default;

            ArrayPartition& operator=(ArrayPartition&&) = default;

            hpx::future<Data> data() const;

            hpx::future<Data> slice(Slices const& slices) const;

            hpx::future<void> fill(Element value);

            hpx::future<void> set_data(Data const& data);

            hpx::future<Offset> offset() const;

            hpx::future<Shape> shape() const;

            hpx::future<Count> nr_elements() const;
    };


    namespace detail {

        template<typename E, Rank r>
        class ArrayTraits<ArrayPartition<E, r>>
        {

            public:

                using Element = E;

                constexpr static Rank rank = r;

                using Offset = typename ArrayPartition<E, r>::Offset;

                using Shape = typename ArrayPartition<E, r>::Shape;

                using Slices = typename ArrayPartition<E, r>::Slices;

                using Slice = typename ArrayPartition<E, r>::Slice;

                template<typename E_, Rank r_>
                using Partition = ArrayPartition<E_, r_>;

                template<typename E_, Rank r_>
                using Data = typename ArrayPartition<E_, r_>::Data;
        };


        template<typename Element, Rank rank>
        class IsArrayPartition<ArrayPartition<Element, rank>>: public std::true_type
        {
        };

    }  // namespace detail
}  // namespace lue
