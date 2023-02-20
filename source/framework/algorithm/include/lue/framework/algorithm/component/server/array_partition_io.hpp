#pragma once
#include "lue/framework/algorithm/detail/array_partition_io.hpp"
#include <hpx/include/components.hpp>


namespace lue::server {

    template<typename Index, Rank rank, typename Value_>
    class ArrayPartitionIO:
        public hpx::components::migration_support<
            hpx::components::component_base<ArrayPartitionIO<Index, rank, Value_>>>
    {

        private:

            using Base = hpx::components::migration_support<
                hpx::components::component_base<ArrayPartitionIO<Index, rank, Value_>>>;

        public:

            using Data = typename detail::ArrayPartitionIO<Index, rank, Value_>;
            using Indices = typename Data::Indices;
            using Value = typename Data::Value;
            using Shape = typename Data::Shape;
            using Offset = typename Data::Offset;
            using PartitionOffsets = typename Data::PartitionOffsets;
            using PartitionOffsetCounts = typename Data::PartitionOffsetCounts;


            ArrayPartitionIO():

                _io{}

            {
            }


            ArrayPartitionIO(Data&& data):

                _io{std::move(data)}

            {
            }


            // Components which should be migrated using hpx::migrate<> need to
            // be Serializable and CopyConstructable. Components can be
            // MoveConstructable in which case the serialized data is moved into the
            // component's constructor.
            ArrayPartitionIO(ArrayPartitionIO const& other):

                Base{other},
                _io{other._io}

            {
            }


            ArrayPartitionIO(ArrayPartitionIO&& other):

                Base{std::move(other)},
                _io{std::move(other._io)}

            {
            }


            bool is_drained() const
            {
                return _io.is_drained();
            }


            bool is_solved() const
            {
                return _io.is_solved();
            }


            Count nr_input_cells() const
            {
                return _io.nr_input_cells();
            }


            PartitionOffsets partition_offsets() const
            {
                return _io.partition_offsets();
            }


            PartitionOffsetCounts partition_offset_counts() const
            {
                return _io.partition_offset_counts();
            }


            std::vector<std::tuple<Indices, Value>> drain(
                Offset const& partition_offset, Shape const& partition_shape)
            {
                return _io.drain(partition_offset, partition_shape);
            }


            Data& data()
            {
                return _io;
            }


            Data const& data() const
            {
                return _io;
            }


            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned version)
            {
                archive& _io;
            }


        private:

            Data _io;


        public:

            HPX_DEFINE_COMPONENT_ACTION(ArrayPartitionIO, is_drained, IsDrainedAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartitionIO, is_solved, IsSolvedAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartitionIO, nr_input_cells, NrInputCellsAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartitionIO, partition_offsets, PartitionOffsetsAction);
            HPX_DEFINE_COMPONENT_ACTION(
                ArrayPartitionIO, partition_offset_counts, PartitionOffsetCountsAction);
            HPX_DEFINE_COMPONENT_ACTION(ArrayPartitionIO, drain, DrainAction);
    };

}  // namespace lue::server


#define LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(Index, rank, Value)                              \
                                                                                                             \
    namespace lue::detail {                                                                                  \
                                                                                                             \
        using ArrayPartitionIO_##Index##_##rank##_##Value = server::ArrayPartitionIO<Index, rank, Value>;    \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::IsDrainedAction,                           \
        ArrayPartitionIO_##Index##_##rank##_##Value##_IsDrainedAction)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::IsSolvedAction,                            \
        ArrayPartitionIO_##Index##_##rank##_##Value##_IsSolvedAction)                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::NrInputCellsAction,                        \
        ArrayPartitionIO_##Index##_##rank##_##Value##_NrInputCellsAction)                                    \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::DrainAction,                               \
        ArrayPartitionIO_##Index##_##rank##_##Value##_DrainAction)                                           \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::PartitionOffsetsAction,                    \
        ArrayPartitionIO_##Index##_##rank##_##Value##_PartitionOffsetsAction)                                \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::ArrayPartitionIO_##Index##_##rank##_##Value::PartitionOffsetCountsAction,               \
        ArrayPartitionIO_##Index##_##rank##_##Value##_PartitionOffsetCountsAction)

using lue_Index = lue::Index;

LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, /* std:: */ uint64_t)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, float)
LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS(lue_Index, 2, double)


// TODO Can we move this registration stuff to a module?
// #undef LUE_REGISTER_ARRAY_PARTITION_IO_ACTION_DECLARATIONS


#define LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(name)                                        \
                                                                                                             \
    namespace lue {                                                                                          \
                                                                                                             \
        template<typename Index, Rank rank, typename Value>                                                  \
        class ArrayPartitionIO##name##Action                                                                 \
        {                                                                                                    \
                                                                                                             \
            public:                                                                                          \
                                                                                                             \
                using Type = typename server::ArrayPartitionIO<Index, rank, Value>::name##Action;            \
        };                                                                                                   \
    }

LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(Drain)
LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(IsDrained)
LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(IsSolved)
LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(NrInputCells)
LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(PartitionOffsets)
LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE(PartitionOffsetCounts)

#undef LUE_DEFINE_ARRAY_PARTITION_IO_COMPONENT_ACTION_TEMPLATE
