#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/offset.hpp"
#include "lue/framework/core/shape.hpp"
#include <hpx/serialization/map.hpp>
#include <hpx/serialization/set.hpp>
#include <hpx/synchronization/mutex.hpp>
#include <algorithm>
#include <vector>


namespace lue::detail {

    template<typename Index, typename Count, Rank rank>
    auto is_within_partition(
        lue::Indices<Index, rank> const& indices, lue::Shape<Count, rank> const& partition_shape) -> bool
    {
        return std::all_of(
            indices.begin(),
            indices.end(),

            [extent_it = partition_shape.begin()](auto const idx) mutable
            {
                bool const result = idx >= 0 && idx < *extent_it;
                ++extent_it;
                return result;
            }

        );
    }


    /*!
        @brief      Class for storing information related to accumulation
                    of material through a partitioned array, according to
                    a flow direction network

        Information stored:
        - Which cells on the border of a partition are the input cells
        - Which cells on the border of a partition are the output cells
        - How much material can be drained from an output cell

        The input cells are set upon construction and cannot added
        to afterwards.
    */
    template<typename Index, Rank rank, typename Value_>
    class ArrayPartitionIO
    {

        public:

            using Indices = lue::Indices<Index, rank>;
            using CellsIdxs = std::vector<Indices>;
            using Shape = lue::Shape<Count, rank>;
            using Offset = lue::Offset<std::int8_t, rank>;
            using PartitionOffsets = std::set<Offset>;
            using PartitionOffsetCounts = std::vector<std::tuple<Offset, Count>>;
            using OffsetMap = std::map<Indices, Offset>;
            using Value = Value_;
            using ValueMap = std::map<Indices, Value>;


            ArrayPartitionIO():

                _partition_shape{},
                _input_cells_idxs{},
                _offsets{},
                _values{},
                _input_cells_idxs_mutex{},
                _offsets_mutex{},
                _values_mutex{}

            {
                std::fill(_partition_shape.begin(), _partition_shape.end(), 0);

                assert_invariants();
            }


            ArrayPartitionIO(Shape const& partition_shape, CellsIdxs const&& input_cells_idxs):

                _partition_shape{partition_shape},
                _input_cells_idxs{input_cells_idxs},
                _offsets{},
                _values{},
                _input_cells_idxs_mutex{},
                _offsets_mutex{},
                _values_mutex{}

            {
                assert_invariants();
            }


            ArrayPartitionIO(ArrayPartitionIO const& other):

                _partition_shape{other._partition_shape}

            {
                std::scoped_lock lock{
                    other._input_cells_idxs_mutex, other._offsets_mutex, other._values_mutex};

                _input_cells_idxs = other._input_cells_idxs;
                _offsets = other._offsets;
                _values = other._values;
            }


            ArrayPartitionIO(ArrayPartitionIO&& other):

                _partition_shape{std::move(other._partition_shape)}

            {
                // std::scoped_lock lock{
                //     other._input_cells_idxs_mutex,
                //     other._offsets_mutex,
                //     other._values_mutex};

                _input_cells_idxs = std::move(other._input_cells_idxs);
                _offsets = std::move(other._offsets);
                _values = std::move(other._values);
            }


            ~ArrayPartitionIO() = default;

            ArrayPartitionIO& operator=(ArrayPartitionIO const&) = delete;

            ArrayPartitionIO& operator=(ArrayPartitionIO&&) = delete;


            /*!
                @brief      Add information about @a output_cell_idxs
                @param      output_cell_idxs Indices of partition output cell. No
                            information for this output cell must already
                            be present.
                @param      offset Offset to input cell in neighbouring
                            partition, relative to the output cell
                @param      value Value related to output cell
            */
            void add_output_cell(Indices const& output_cell_idxs, Offset&& offset, Value&& value)
            {
                std::scoped_lock lock{_offsets_mutex, _values_mutex};

                lue_hpx_assert(!has_offset(output_cell_idxs));
                lue_hpx_assert(!has_value(output_cell_idxs));

                _offsets.emplace(std::make_pair(output_cell_idxs, std::move(offset)));
                _values.emplace(std::make_pair(output_cell_idxs, std::move(value)));

                assert_invariants();

                lue_hpx_assert(has_offset(output_cell_idxs));
                lue_hpx_assert(has_value(output_cell_idxs));
            }


            void add_output_cell(Indices const& output_cell_idxs, Offset const& offset, Value const& value)
            {
                std::scoped_lock lock{_offsets_mutex, _values_mutex};

                lue_hpx_assert(!has_offset(output_cell_idxs));
                lue_hpx_assert(!has_value(output_cell_idxs));

                _offsets.emplace(std::make_pair(output_cell_idxs, offset));
                _values.emplace(std::make_pair(output_cell_idxs, value));

                assert_invariants();

                lue_hpx_assert(has_offset(output_cell_idxs));
                lue_hpx_assert(has_value(output_cell_idxs));
            }


            bool contains_input_cell(Indices const& input_cell_idxs)
            {
                std::scoped_lock lock{_input_cells_idxs_mutex};

                return std::find(_input_cells_idxs.begin(), _input_cells_idxs.end(), input_cell_idxs) !=
                       _input_cells_idxs.end();
            }


            void remove_input_cell(Indices const& input_cell_idxs)
            {
                std::scoped_lock lock{_input_cells_idxs_mutex};

                // Call this method after material for the input cell has
                // been pushed into the partition, possibly to an
                // output cell

                auto it = std::find(_input_cells_idxs.begin(), _input_cells_idxs.end(), input_cell_idxs);
                lue_hpx_assert(it != _input_cells_idxs.end());
                _input_cells_idxs.erase(it);
            }


            /*!
                @brief      Return whether all input cells have received
                            values (and are removed) by now

                A solved partition does not need to wait for upstream
                material anymore.

                The fact that a partition is solved does not imply that
                all output cells have drained already. If output cells
                are still present, then these can be drained.

                Input cells get solved (and removed), output cells
                get drained (and removed).
            */
            bool is_solved() const
            {
                std::scoped_lock lock{_input_cells_idxs_mutex};

                return _input_cells_idxs.empty();
            }


            Count nr_input_cells() const
            {
                std::scoped_lock lock{_input_cells_idxs_mutex};

                return _input_cells_idxs.size();
            }


            /*!
                @brief      Return whether all output cells have been
                            drained (and are removed)

                Draining has to do with providing values from partition
                output cells to partition input cells of partition
                downstream cells (solving them).

                Input cells get solved (and removed), output cells
                get drained (and removed).
            */
            bool is_drained() const
            {
                std::scoped_lock lock{_offsets_mutex};

                return _offsets.empty();
            }


            /*!
                @brief      Return a collection of offsets to downstream
                            partitions this instance drains towards
                @warning    It is assumed that this function is only
                            called when is_solved() returns
                            true and is_drained() returns false.
            */
            PartitionOffsets partition_offsets() const
            {
                // Downstream partition offsets.

                // No, not every partition has to be / can be fully
                // solved. A stream can flow out of a partition and flow
                // back into it again. To make progress, some partitions
                // have to be partly drained.
                // lue_hpx_assert(is_solved());

                // No, not every partition drains. Well, most do, but
                // in theory, and in our tests, a partition can drain into
                // itself, into a sink.
                // lue_hpx_assert(!is_drained());

                PartitionOffsets result{};

                // Iterate over all output cells and determine the
                // offset towards the neighbouring partitions to which the
                // material flows

                // The offset stored is the offset relative to the
                // output cell itself. Here we want an offset relative to
                // the partition the output cell is part of.

                std::scoped_lock lock{_offsets_mutex};

                for (auto const& [output_cell_idxs, cell_offset] : _offsets)
                {
                    result.insert(partition_offset(output_cell_idxs, cell_offset));
                }

                return result;
            }


            PartitionOffsetCounts partition_offset_counts() const
            {
                std::vector<Offset> offsets{};
                std::vector<Count> counts{};

                {
                    Offset offset;
                    std::scoped_lock lock{_offsets_mutex};

                    for (auto const& [output_cell_idxs, cell_offset] : _offsets)
                    {
                        offset = partition_offset(output_cell_idxs, cell_offset);

                        auto it = std::find(offsets.begin(), offsets.end(), offset);

                        if (it == offsets.end())
                        {
                            offsets.push_back(std::move(offset));
                            counts.push_back(1);
                        }
                        else
                        {
                            ++counts[std::distance(offsets.begin(), it)];
                        }
                    }
                }

                PartitionOffsetCounts offset_counts(offsets.size());

                for (std::size_t i = 0; i < offsets.size(); ++i)
                {
                    offset_counts[i] = std::make_tuple(offsets[i], counts[i]);
                }

                return offset_counts;
            }


            std::vector<std::tuple<Indices, Value>> drain(
                Offset const& partition_offset, Shape const& partition_shape)
            {
                // For each solved output cell that drains to the
                // partition at the offset passed in:
                // - Determine the indices of the input cell in the
                //     neighbouring partition
                // - Copy the value that can be forwarded
                // - Remove the output cell from this instance
                //
                // Return a collection of the <indices, value> tuples.

                // No, it must be possible to partly drain unsolved
                // partitions. It may require multiply steps to fully
                // drain a partition.
                // lue_hpx_assert(is_solved());

                lue_hpx_assert(!is_drained());
                lue_hpx_assert(std::all_of(
                    partition_offset.begin(),
                    partition_offset.end(),
                    [](auto const& idx) { return idx == -1 || idx == 0 || idx == 1; }));

                std::vector<std::tuple<Indices, Value>> input_cells;
                std::vector<Indices> output_cells;

                std::scoped_lock lock{_input_cells_idxs_mutex, _offsets_mutex, _values_mutex};

                // Iterate over all output cells
                for (auto const& [output_cell_idxs, cell_offset] : _offsets)
                {
                    // If partition drains to the partition at the offset passed in ...
                    if (this->partition_offset(output_cell_idxs, cell_offset) == partition_offset)
                    {
                        // ... determine the input cell indices in the
                        // neighbouring partition, and copy the value.
                        input_cells.push_back(std::make_tuple(
                            input_cell(output_cell_idxs, partition_shape), value(output_cell_idxs)));
                        output_cells.push_back(output_cell_idxs);
                    }
                }

                // Remove all output cells found from the instance
                for (auto const& output_cell_idxs : output_cells)
                {
                    remove_output_cell(output_cell_idxs);
                }

                // No, we have just drained to one of the downstream
                // partitions. Not all of them.
                // lue_hpx_assert(is_drained());

                return input_cells;
            }


            Shape const& partition_shape() const
            {
                // Only used for testing
                return _partition_shape;
            }


            CellsIdxs input_cells_idxs() const
            {
                // Only used for testing
                std::scoped_lock lock{_input_cells_idxs_mutex};

                return _input_cells_idxs;
            }


            OffsetMap offsets() const
            {
                // Only used for testing
                std::scoped_lock lock{_offsets_mutex};

                return _offsets;
            }


            ValueMap values() const
            {
                // Only used for testing
                std::scoped_lock lock{_values_mutex};

                return _values;
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned version)
            {
                // std::scoped_lock lock{
                //     _input_cells_idxs_mutex,
                //     _offsets_mutex,
                //     _values_mutex};

                assert_invariants();

                // clang-format off
                archive & _partition_shape & _input_cells_idxs & _offsets & _values;
                // clang-format on

                assert_invariants();
            }


            bool has_offset(Indices const& output_cell_idxs) const
            {
                return _offsets.find(output_cell_idxs) != _offsets.end();
            }


            Offset const& offset(Indices const output_cell_idxs) const
            {
                lue_hpx_assert(has_offset(output_cell_idxs));

                return _offsets.at(output_cell_idxs);
            }


            bool has_value(Indices const& output_cell_idxs) const
            {
                return _values.find(output_cell_idxs) != _values.end();
            }


            Value const& value(Indices const output_cell_idxs) const
            {
                lue_hpx_assert(has_value(output_cell_idxs));

                return _values.at(output_cell_idxs);
            }


            Indices input_cell(Indices const output_cell_idxs, Shape const& partition_shape)
            {
                Offset const& offset{this->offset(output_cell_idxs)};
                Indices result{};

                for (Rank d = 0; d < rank; ++d)
                {
                    if (output_cell_idxs[d] == 0 && offset[d] == -1)
                    {
                        // Along this dimension the offset points into
                        // the previous partition. Jump to it.
                        result[d] = partition_shape[d] - 1;
                    }
                    else if (output_cell_idxs[d] == _partition_shape[d] - 1 && offset[d] == 1)
                    {
                        // Along this dimension the offset points into
                        // the next partition. Jump to it.
                        result[d] = 0;
                    }
                    else
                    {
                        result[d] = output_cell_idxs[d] + offset[d];
                    }
                }

                lue_hpx_assert(is_within_partition(result, partition_shape));

                return result;
            }


            void remove_output_cell(Indices const& output_cell_idxs)
            {
                [[maybe_unused]] std::size_t nr_erased;

                nr_erased = _offsets.erase(output_cell_idxs);
                lue_hpx_assert(nr_erased == 1);

                nr_erased = _values.erase(output_cell_idxs);
                lue_hpx_assert(nr_erased == 1);

                assert_invariants();
            }


            Offset partition_offset(Indices const& output_cell_idxs, Offset const& cell_offset) const
            {
                Offset partition_offset{};

                for (Rank d = 0; d < rank; ++d)
                {
                    if ((output_cell_idxs[d] == 0) && cell_offset[d] == -1)
                    {
                        partition_offset[d] = -1;
                    }
                    else if ((output_cell_idxs[d] == _partition_shape[d] - 1) && cell_offset[d] == 1)
                    {
                        partition_offset[d] = 1;
                    }
                    else
                    {
                        partition_offset[d] = 0;
                    }
                }

                return partition_offset;
            }


            Offset partition_offset(Indices const& output_cell_idxs) const
            {
                return partition_offset(output_cell_idxs, offset(output_cell_idxs));
            }


            void assert_invariants() const
            {
#ifndef NDEBUG
                lue_hpx_assert(_offsets.size() == _values.size());

                if (std::all_of(
                        _partition_shape.begin(),
                        _partition_shape.end(),
                        [](Count const extent) { return extent == 0; }))
                {
                    lue_hpx_assert(_input_cells_idxs.empty());
                    lue_hpx_assert(_offsets.empty());
                    lue_hpx_assert(_values.empty());
                }


                auto on_partition_border = [partition_shape = _partition_shape](Indices const& indices)
                {
                    return std::any_of(
                        indices.begin(),
                        indices.end(),

                        [extent_it = partition_shape.begin()](auto const idx) mutable
                        {
                            bool const result = idx == 0 || idx == (*extent_it) - 1;
                            ++extent_it;
                            return result;
                        }

                    );
                };


                for (auto const& input_cell_idxs : _input_cells_idxs)
                {
                    lue_hpx_assert(is_within_partition(input_cell_idxs, _partition_shape));
                    lue_hpx_assert(on_partition_border(input_cell_idxs));
                }


                auto points_towards_neighbour =
                    [partition_shape = _partition_shape](Indices const& indices, Offset const& offset)
                {
                    for (Rank d = 0; d < rank; ++d)
                    {
                        // Along at least one of the dimensions the
                        // offset must be pointing towards a
                        // neighbouring partition
                        if ((indices[d] == 0 && offset[d] < 0) ||
                            (indices[d] == partition_shape[d] - 1 && offset[d] > 0))
                        {
                            return true;
                        }
                    }

                    return false;
                };

                for (auto const& [output_cell, offset] : _offsets)
                {
                    // Verify that the offset is pointing outwards,
                    // towards a neighbouring partition

                    lue_hpx_assert(points_towards_neighbour(output_cell, offset));
                }
#endif
            }


            Shape _partition_shape;

            CellsIdxs _input_cells_idxs;

            //! Map of offset to input cell in neighbouring partition by output cell index
            OffsetMap _offsets;

            //! Map of value instances per output cell index
            ValueMap _values;

            mutable hpx::mutex _input_cells_idxs_mutex;
            mutable hpx::mutex _offsets_mutex;
            mutable hpx::mutex _values_mutex;
    };

}  // namespace lue::detail
