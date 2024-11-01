#pragma once
#include "lue/framework/algorithm/component/server/array_partition_io.hpp"


namespace lue {

    template<typename Index, Rank rank, typename Value_>
    class ArrayPartitionIO:
        public hpx::components::
            client_base<ArrayPartitionIO<Index, rank, Value_>, server::ArrayPartitionIO<Index, rank, Value_>>

    {

        public:

            using Server = server::ArrayPartitionIO<Index, rank, Value_>;
            using Base = hpx::components::client_base<ArrayPartitionIO<Index, rank, Value_>, Server>;
            using Data = typename Server::Data;
            using Indices = typename Server::Indices;
            using Value = typename Server::Value;
            using Shape = typename Data::Shape;
            using Offset = typename Server::Offset;
            using PartitionOffsets = typename Server::PartitionOffsets;
            using PartitionOffsetCounts = typename Server::PartitionOffsetCounts;


            ArrayPartitionIO() = default;


            explicit ArrayPartitionIO(hpx::id_type const& component_id):

                Base{component_id}

            {
            }


            explicit ArrayPartitionIO(hpx::future<hpx::id_type>&& component_id):

                Base{std::move(component_id)}

            {
            }


            ArrayPartitionIO(hpx::future<ArrayPartitionIO>&& partition_io):

                Base{std::move(partition_io)}

            {
            }


            ArrayPartitionIO(hpx::id_type const locality_id, Data&& data):

                Base{hpx::new_<Server>(locality_id, std::move(data))}

            {
            }


            ArrayPartitionIO(ArrayPartitionIO const&) = default;

            ArrayPartitionIO(ArrayPartitionIO&&) = default;

            ~ArrayPartitionIO() = default;

            auto operator=(ArrayPartitionIO const&) -> ArrayPartitionIO& = default;

            auto operator=(ArrayPartitionIO&&) -> ArrayPartitionIO& = default;


            hpx::future<bool> is_drained() const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::IsDrainedAction action;

                return hpx::async(action, this->get_id());
            }


            hpx::future<bool> is_solved() const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::IsSolvedAction action;

                return hpx::async(action, this->get_id());
            }


            hpx::future<Count> nr_input_cells() const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::NrInputCellsAction action;

                return hpx::async(action, this->get_id());
            }


            auto partition_offsets() const -> hpx::future<PartitionOffsets>
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::PartitionOffsetsAction action;

                return hpx::async(action, this->get_id());
            }


            auto partition_offset_counts() const -> hpx::future<PartitionOffsetCounts>
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::PartitionOffsetCountsAction action;

                return hpx::async(action, this->get_id());
            }


            auto drain(Offset const& partition_offset, Shape const& partition_shape)
                -> hpx::future<std::vector<std::tuple<Indices, Value>>>
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename Server::DrainAction action;

                return hpx::async(action, this->get_id(), partition_offset, partition_shape);
            }
    };


    namespace detail {

        template<typename Index, Rank rank_, typename Value>
        class ArrayTraits<lue::ArrayPartitionIO<Index, rank_, Value>>
        {

            public:

                using Element = Value;

                constexpr static Rank rank = rank_;

                using Offset = typename lue::ArrayPartitionIO<Index, rank, Value>::Offset;

                using Shape = typename lue::ArrayPartitionIO<Index, rank, Value>::Shape;

                template<typename E_, Rank r_>
                using Data = typename lue::ArrayPartitionIO<Index, r_, E_>::Data;
        };

    }  // namespace detail
}  // namespace lue


#define LUE_REGISTER_ARRAY_PARTITION_IO_ACTIONS(Index, rank, Value)                                          \
                                                                                                             \
    namespace lue::detail {                                                                                  \
                                                                                                             \
        using ArrayPartitionIOServerType_##Index##_##rank##_##Value =                                        \
            hpx::components::component<ArrayPartitionIO_##Index##_##rank##_##Value>;                         \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::ArrayPartitionIOServerType_##Index##_##rank##_##Value,                                  \
        ArrayPartitionIOServer_##Index##_##rank##_##Value)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::DrainAction,                               \
        ArrayPartitionIO_##Index##_##rank##_##Value##_DrainAction)                                           \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::IsDrainedAction,                           \
        ArrayPartitionIO_##Index##_##rank##_##Value##_IsDrainedAction)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::IsSolvedAction,                            \
        ArrayPartitionIO_##Index##_##rank##_##Value##_IsSolvedAction)                                        \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::PartitionOffsetsAction,                    \
        ArrayPartitionIO_##Index##_##rank##_##Value##_PartitionOffsetsAction)                                \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::PartitionOffsetCountsAction,               \
        ArrayPartitionIO_##Index##_##rank##_##Value##_PartitionOffsetCountsAction)
