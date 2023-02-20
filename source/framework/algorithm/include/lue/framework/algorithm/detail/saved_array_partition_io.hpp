// #pragma once
// #include "lue/framework/core/indices.hpp"
// #include "lue/framework/core/offset.hpp"
// #include "lue/framework/core/shape.hpp"
// #include "lue/assert.hpp"
// #include <hpx/serialization/map.hpp>
// #include <hpx/serialization/set.hpp>
// #include <algorithm>
// #include <map>
// #include <set>
// #include <vector>
//
//
// namespace lue::detail {
//
//     template<
//         typename Index,
//         typename Count,
//         Rank rank>
//     bool is_within_partition(
//         lue::Indices<Index, rank> const& indices,
//         lue::Shape<Count, rank> const& partition_shape)
//     {
//         return std::all_of(indices.begin(), indices.end(),
//
//                 [extent_it=partition_shape.begin()](
//                     auto const idx) mutable
//                 {
//                     bool const result = idx >= 0 && idx < *extent_it;
//                     ++extent_it;
//                     return result;
//                 }
//
//             );
//     }
//
//
//
//     /*!
//         @brief      Class for storing information related to accumulation
//                     of material through a partitioned array, according to
//                     a flow direction network
//
//         Instances can be stored in a partitioned array. For each partition
//         a single instance.
//
//         The input cells whose indices are stored are those input cells for
//         which state still has to be accumulated through the network towards
//         the output cell. These input and output cells are not 'solved yet',
//         they are 'dirty'. Once state has passed through an input cell,
//         it should be removed from an output cell's collection. Once all
//         input cells have been removed, the output cell is 'solved'. Its value
//         is the final result, which can be forwarded to the input cell of a
//         neighbouring partition. After that, the output cell can also be
//         removed.
//     */
//     template<
//         typename Index,
//         Rank rank,
//         typename Value_>
//     class ArrayPartitionIO
//     {
//
//         public:
//
//             using Indices = lue::Indices<Index, rank>;
//             using CellsIdxs = std::vector<Indices>;
//             using CellsIdxsMap = std::map<Indices, CellsIdxs>;
//             using Shape = lue::Shape<Count, rank>;
//             using Offset = lue::Offset<std::int8_t, rank>;
//             using Offsets = std::vector<Offset>;
//             using PartitionOffsets = std::set<Offset>;
//             using OffsetMap = std::map<Indices, Offset>;
//             using Value = Value_;
//             using ValueMap = std::map<Indices, Value>;
//
//
//             ArrayPartitionIO():
//
//                 _partition_shape{},
//                 _input_cells_idxs{},
//                 _offsets{},
//                 _values{}
//
//             {
//                 std::fill(_partition_shape.begin(), _partition_shape.end(), 0);
//
//                 assert_invariants();
//             }
//
//
//             ArrayPartitionIO(
//                 Shape const& partition_shape):
//
//                 _partition_shape{partition_shape},
//                 _input_cells_idxs{},
//                 _offsets{},
//                 _values{}
//
//             {
//                 assert_invariants();
//             }
//
//
//             bool operator==(
//                 ArrayPartitionIO const& other) const
//             {
//                 return
//                     _input_cells_idxs == other._input_cells_idxs &&
//                     _offsets == other._offsets &&
//                     _values == other._values;
//             }
//
//
//             bool operator!=(
//                 ArrayPartitionIO const& other) const
//             {
//                 return !(*this == other);
//             }
//
//
//             /*!
//                 @brief      Return number of output cells stored in the instance
//             */
//             std::size_t nr_output_cells() const
//             {
//                 lue_hpx_assert(_input_cells_idxs.size() == _values.size());
//                 lue_hpx_assert(_offsets.size() == _values.size());
//
//                 return _input_cells_idxs.size();
//             }
//
//
//             /*!
//                 @brief      Add information about @a output_cell_idxs
//                 @param      output_cell_idxs Indices of partition output cell. No
//                             information for this output cell must already
//                             be present.
//                 @param      input_cells_idxs Collection of indices of
//                             partition input cells. These indices must already
//                             be sorted. No index must occur more than once
//                             in the collection.
//                 @param      value Value related with output cell
//
//                 It is assumed that the collection of input cell indices and
//                 the value are not needed anymore at the call site. They are
//                 moved into the instance.
//             */
//             void add(
//                 Indices const output_cell_idxs,
//                 CellsIdxs&& input_cells_idxs,
//                 Offset&& offset,
//                 Value&& value)
//             {
//                 lue_hpx_assert(!has_input_cells(output_cell_idxs));
//                 lue_hpx_assert(!has_value(output_cell_idxs));
//                 lue_hpx_assert(std::is_sorted(input_cells_idxs.begin(), input_cells_idxs.end()));
//                 /// lue_hpx_assert(
//                 ///     std::adjacent_find(input_cells_idxs.begin(), input_cells_idxs.end()) ==
//                 input_cells_idxs.end());
//
//                 _input_cells_idxs.emplace(std::make_pair(output_cell_idxs, std::move(input_cells_idxs)));
//                 _offsets.emplace(std::make_pair(output_cell_idxs, std::move(offset)));
//                 _values.emplace(std::make_pair(output_cell_idxs, std::move(value)));
//
//                 assert_invariants();
//
//                 lue_hpx_assert(has_input_cells(output_cell_idxs));
//                 lue_hpx_assert(has_value(output_cell_idxs));
//             }
//
//
//             /*!
//                 @brief      Remove @a input_cell_idx from the collection of
//                             partition input cell indices related to @a output_cell_idxs
//                 @param      output_cell_idxs Indices of partition output
//                             cell. Information for this output cell must be
//                             present in the instance.
//                 @param      input_cell_idx Indices of partition input cell. This
//                             index must be present in the instance.
//             */
//             void remove_input_cell(
//                 Indices const output_cell_idxs,
//                 Indices const input_cell_idx)
//             {
//                 auto& input_cells_idxs{this->input_cells_idxs(output_cell_idxs)};
//
//                 lue_hpx_assert(std::count(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx)
//                 == 1);
//
//                 input_cells_idxs.erase(
//                     std::lower_bound(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx));
//
//                 lue_hpx_assert(std::count(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx)
//                 == 0);
//             }
//
//
//             void remove_output_cell(
//                 Indices const& output_cell_idxs)
//             {
//                 lue_hpx_assert(is_solved(output_cell_idxs));
//
//                 [[maybe_unused]] std::size_t nr_erased;
//
//                 nr_erased = _input_cells_idxs.erase(output_cell_idxs);
//                 lue_hpx_assert(nr_erased == 1);
//
//                 nr_erased = _offsets.erase(output_cell_idxs);
//                 lue_hpx_assert(nr_erased == 1);
//
//                 nr_erased = _values.erase(output_cell_idxs);
//                 lue_hpx_assert(nr_erased == 1);
//
//                 assert_invariants();
//             }
//
//
//             /*!
//                 @brief      Update the value associated with the partition
//                             output cell with index @a output_cell_idxs
//                 @param      output_cell_idxs Indices of partition output
//                             cell. Information for this output cell must be
//                             present in the instance.
//                 @param      value New value
//             */
//             void update_value(
//                 Indices const output_cell_idxs,
//                 Value&& value)
//             {
//                 lue_hpx_assert(has_value(output_cell_idxs));
//
//                 _values.at(output_cell_idxs) = std::move(value);
//             }
//
//
//             void accumulate_value(
//                 Indices const output_cell_idxs,
//                 Value&& value)
//             {
//                 lue_hpx_assert(has_value(output_cell_idxs));
//
//                 _values.at(output_cell_idxs) += std::move(value);
//             }
//
//
//             CellsIdxs const& input_cells(
//                 Indices const output_cell_idxs) const
//             {
//                 lue_hpx_assert(has_input_cells(output_cell_idxs));
//
//                 return _input_cells_idxs.at(output_cell_idxs);
//             }
//
//
//             Offset const& offset(
//                 Indices const output_cell_idxs) const
//             {
//                 lue_hpx_assert(has_value(output_cell_idxs));
//
//                 return _offsets.at(output_cell_idxs);
//             }
//
//
//             Value const& value(
//                 Indices const output_cell_idxs) const
//             {
//                 lue_assert(has_value(output_cell_idxs));
//
//                 return _values.at(output_cell_idxs);
//             }
//
//
//             CellsIdxsMap const& input_cells_idxs() const
//             {
//                 return _input_cells_idxs;
//             }
//
//
//             ValueMap const& values() const
//             {
//                 return _values;
//             }
//
//
//             bool is_solved(
//                 Indices const& output_cell_idxs) const
//             {
//                 return _input_cells_idxs.at(output_cell_idxs).empty();
//             }
//
//
//             bool is_unsolved() const
//             {
//                 // Return whether any output cell has unsolved input cells
//                 return std::any_of(
//                         _input_cells_idxs.begin(), _input_cells_idxs.end(),
//
//                         [](auto const& pair)
//                         {
//                             return !pair.second.empty();
//                         }
//
//                     );
//             }
//
//
//             bool is_solved() const
//             {
//                 return !is_unsolved();
//             }
//
//
//             Offsets offsets() const
//             {
//                 // Iterate over all output cells and copy the offsets into
//                 // the collection
//
//                 Offsets offsets{};
//                 offsets.reserve(_offsets.size());
//
//                 for(auto const& pair: _offsets)
//                 {
//                     offsets.push_back(pair.second);
//                 }
//
//                 return offsets;
//             }
//
//
//             Offset partition_offset(
//                 Indices const& output_cell_idxs,
//                 Offset const& cell_offset) const
//             {
//                 Offset partition_offset{};
//
//                 for(Rank d = 0; d < rank; ++d)
//                 {
//                     if((output_cell_idxs[d] == 0) || (output_cell_idxs[d] == _partition_shape[d] - 1))
//                     {
//                         partition_offset[d] = cell_offset[d];
//                     }
//                     else
//                     {
//                         partition_offset[d] = 0;
//                     }
//                 }
//
//                 return partition_offset;
//             }
//
//
//             Offset partition_offset(
//                 Indices const& output_cell_idxs) const
//             {
//                 return partition_offset(output_cell_idxs, offset(output_cell_idxs));
//             }
//
//
//             PartitionOffsets partition_offsets() const
//             {
//                 PartitionOffsets result{};
//
//                 // Iterate over all output cells and determine the
//                 // offset towards the neighbouring partitions to which the
//                 // material flows
//
//                 // The offset stored is the offset relative to the
//                 // output cell itself. Here we want an offset relative to
//                 // the partition the output cell is part of.
//
//                 for(auto const& [output_cell_idxs, cell_offset]: _offsets)
//                 {
//                     result.insert(partition_offset(output_cell_idxs, cell_offset));
//                 }
//
//                 return result;
//             }
//
//
//             Indices input_cell(
//                 Indices const output_cell_idxs,
//                 Shape const& partition_shape)
//             {
//                 Offset const& offset{this->offset(output_cell_idxs)};
//                 Indices result{};
//
//                 for(Rank d = 0; d < rank; ++d)
//                 {
//                     if(output_cell_idxs[d] == 0)
//                     {
//                         // First border (e.g. first row)
//                         lue_assert(offset[d] == 0 || offset[d] == -1);
//
//                         if(offset[d] == 0)
//                         {
//                             result[d] = 0;
//                         }
//                         else
//                         {
//                             result[d] = partition_shape[d] - 1;
//                         }
//                     }
//                     else if(output_cell_idxs[d] == _partition_shape[d] - 1)
//                     {
//                         // Last border (e.g. last row)
//                         lue_assert(offset[d] == 0 || offset[d] == 1);
//
//                         if(offset[d] == 0)
//                         {
//                             result[d] = partition_shape[d] - 1;
//                         }
//                         else
//                         {
//                             result[d] = 0;
//                         }
//                     }
//                     else
//                     {
//                         // Any other cell (e.g. between first and last row)
//                         result[d] = output_cell_idxs[d] + offset[d];
//                     }
//                 }
//
//                 lue_assert(is_within_partition(result, partition_shape));
//
//                 return result;
//             }
//
//
//             std::vector<std::tuple<Indices, Value>> drain(
//                 Offset const& partition_offset,
//                 Shape const& partition_shape)
//             {
//                 // For each solved output cell that drains to the
//                 // partition at the offset passed in:
//                 // - Determine the indices of the input cell in the
//                 //     neighbouring partition
//                 // - Copy the value that can be forwarded
//                 // - Remove the output cell from this instance
//                 //
//                 // Return a collection of the <indices, value> tuples.
//
//                 lue_assert(std::all_of(partition_offset.begin(), partition_offset.end(),
//                         [](auto const& idx)
//                         {
//                             return idx == -1 || idx == 0 || idx == 1;
//                         }));
//
//                 std::vector<std::tuple<Indices, Value>> input_cells;
//                 std::vector<Indices> output_cells;
//
//                 // Iterate over all output cells
//                 for(auto const& [output_cell_idxs, input_cells_idxs]: _input_cells_idxs)
//                 {
//                     // If this output cell is solved...
//                     if(input_cells_idxs.empty())
//                     {
//                         // ... and it drains to the partition at the offset passed in ...
//                         if(this->partition_offset(output_cell_idxs) == partition_offset)
//                         {
//                             // ... determine the input cell indices in the
//                             // neighbouring partition, and copy the value.
//                             input_cells.push_back(std::make_tuple(
//                                 input_cell(output_cell_idxs, partition_shape),
//                                 value(output_cell_idxs)));
//                             output_cells.push_back(output_cell_idxs);
//                         }
//                     }
//                 }
//
//                 // Remove all output cells found from the instance
//                 for(auto const& output_cell_idxs: output_cells)
//                 {
//                     remove_output_cell(output_cell_idxs);
//                 }
//
//                 return input_cells;
//             }
//
//
//         private:
//
//             friend class hpx::serialization::access;
//
//             void serialize(
//                 hpx::serialization::input_archive& archive,
//                 [[maybe_unused]] unsigned int const version)
//             {
//                 archive & _input_cells_idxs & _offsets & _values;
//
//                 assert_invariants();
//             }
//
//             void serialize(
//                 hpx::serialization::output_archive& archive,
//                 [[maybe_unused]] unsigned int const version) const
//             {
//                 assert_invariants();
//
//                 archive & _input_cells_idxs & _offsets & _values;
//             }
//
//             bool has_input_cells(
//                 Indices const output_cell_idxs) const
//             {
//                 return _input_cells_idxs.find(output_cell_idxs) != _input_cells_idxs.end();
//             }
//
//             bool has_value(
//                 Indices const output_cell_idxs) const
//             {
//                 return _values.find(output_cell_idxs) != _values.end();
//             }
//
//             CellsIdxs& input_cells_idxs(
//                 Indices const output_cell_idxs)
//             {
//                 lue_assert(has_input_cells(output_cell_idxs));
//
//                 return _input_cells_idxs.at(output_cell_idxs);
//             }
//
//
//             void assert_invariants() const
//             {
// #ifndef NDEBUG
//                 // auto is_within_partition =
//                 //     [partition_shape=_partition_shape](
//                 //         Indices const& indices)
//                 //     {
//                 //         return std::all_of(indices.begin(), indices.end(),
//
//                 //                 [extent_it=partition_shape.begin()](
//                 //                     auto const idx) mutable
//                 //                 {
//                 //                     bool const result = idx >= 0 && idx < *extent_it;
//                 //                     ++extent_it;
//                 //                     return result;
//                 //                 }
//
//                 //             );
//                 //     };
//
//
//                 auto on_partition_border =
//                     [partition_shape=_partition_shape](
//                         Indices const& indices)
//                     {
//                         return std::any_of(indices.begin(), indices.end(),
//
//                                 [extent_it=partition_shape.begin()](
//                                     auto const idx) mutable
//                                 {
//                                     bool const result = idx == 0 || idx == (*extent_it) - 1;
//                                     ++extent_it;
//                                     return result;
//                                 }
//
//                             );
//                     };
//
//                 for(auto const& [output_cell, input_cells]: _input_cells_idxs)
//                 {
//                     // Verify that each output cell is on the partition's border
//                     lue_assert(is_within_partition(output_cell, _partition_shape));
//                     lue_assert(on_partition_border(output_cell));
//
//                     // Verify that each input cell is on the partition's border
//                     for(auto const input_cell: input_cells)
//                     {
//                         lue_assert(is_within_partition(input_cell, _partition_shape));
//                         lue_assert(on_partition_border(input_cell));
//                     }
//                 }
//
//
//                 auto points_towards_neighbour =
//                     [partition_shape=_partition_shape](
//                         Indices const& indices,
//                         Offset const& offset)
//                     {
//                         for(Rank d = 0; d < rank; ++d)
//                         {
//                             // Along at least one of the dimensions the
//                             // offset must be pointing towards a
//                             // neighbouring partition
//                             if(
//                                 (indices[d] == 0 && offset[d] < 0) ||
//                                 (indices[d] == partition_shape[d] - 1 && offset[d] > 0))
//                             {
//                                 return true;
//                             }
//                         }
//
//                         return false;
//                     };
//
//                 for(auto const& [output_cell, offset]: _offsets)
//                 {
//                     // Verify that the offset is pointing towards a
//                     // neighbouring partition
//
//                     lue_assert(points_towards_neighbour(output_cell, offset));
//                 }
// #endif
//             }
//
//
//             Shape _partition_shape;
//
//             //! Map of collection of sorted input cell indices by output cell index
//             CellsIdxsMap _input_cells_idxs;
//
//             //! Map of offset to input cell in neighbouring partition by output cell index
//             OffsetMap _offsets;
//
//             //! Map of value instances per output cell index
//             ValueMap _values;
//
//     };
//
// }  // namespace lue::detail
