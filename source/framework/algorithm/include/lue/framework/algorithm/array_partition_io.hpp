#pragma once
#include "lue/framework/core/indices.hpp"
#include "lue/assert.hpp"
#include <hpx/serialization/map.hpp>
#include <algorithm>
#include <map>
#include <vector>


namespace lue {

/*!
    @brief      Class for storing information related to accumulation
                of material through a partitioned array, according to
                a flow direction network

    Instances can be stored in a partitioned array. For each partition
    a single instance.

    The indices stored here are linear indices of cells in an array
    partition [0, nr_cells_in_partition).

    The input cells whose indices are stored are those input cells for
    which state still has to be accumulated through the network towards
    the output cell. These input and output cells are not 'solved yet',
    they are 'dirty'. Once state has passed through an input cell,
    it should be removed from an output cell's collection. Once all
    input cells have been removed, the output cell is 'solved'. Its value
    is the final result, which can be forwarded to the input cell of a
    neighbouring partition. After that, the output cell can also be
    removed.
*/
template<
    typename Index,
    Rank rank,
    typename Value_>
class ArrayPartitionIO
{

    public:

        using Indices = lue::Indices<Index, rank>;
        using CellsIdxs = std::vector<Indices>;
        using CellsIdxsMap = std::map<Indices, CellsIdxs>;
        using Value = Value_;
        using ValueMap = std::map<Indices, Value>;

        bool operator==(
            ArrayPartitionIO const& other) const
        {
            return
                _input_cells_idxs == other._input_cells_idxs &&
                _values == other._values;
        }

        bool operator!=(
            ArrayPartitionIO const& other) const
        {
            return !(*this == other);
        }

        /*!
            @brief      Return number of output cells stored in the instance
        */
        std::size_t nr_output_cells() const
        {
            lue_assert(_input_cells_idxs.size() == _values.size());

            return _input_cells_idxs.size();
        }

        /*!
            @brief      Add information about @a output_cell_idxs
            @param      output_cell_idxs Indices of partition output cell. No
                        information for this output cell must already
                        be present.
            @param      input_cells_idxs Collection of indices of
                        partition input cells. These indices must already
                        be sorted. No index must occur more than once
                        in the collection.
            @param      value Value related with output cell

            It is assumed that the collection of input cell indices and
            the value are not needed anymore at the call site. They are
            moved into the instance.
        */
        void add(
            Indices const output_cell_idxs,
            CellsIdxs&& input_cells_idxs,
            Value&& value)
        {
            lue_assert(!has_input_cells(output_cell_idxs));
            lue_assert(!has_value(output_cell_idxs));
            lue_assert(std::is_sorted(input_cells_idxs.begin(), input_cells_idxs.end()));
            /// lue_assert(
            ///     std::adjacent_find(input_cells_idxs.begin(), input_cells_idxs.end()) == input_cells_idxs.end());

            _input_cells_idxs.emplace(std::make_pair(output_cell_idxs, std::move(input_cells_idxs)));
            _values.emplace(std::make_pair(output_cell_idxs, std::move(value)));

            lue_assert(has_input_cells(output_cell_idxs));
            lue_assert(has_value(output_cell_idxs));
        }

        /*!
            @brief      Remove @a input_cell_idx from the collection of
                        partition input cell indices related to @a output_cell_idxs
            @param      output_cell_idxs Indices of partition output
                        cell. Information for this output cell must be
                        present in the instance.
            @param      input_cell_idx Indices of partition input cell. This
                        index must be present in the instance.
        */
        void remove_input_cell(
            Indices const output_cell_idxs,
            Indices const input_cell_idx)
        {
            auto& input_cells_idxs{this->input_cells_idxs(output_cell_idxs)};

            lue_assert(std::count(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx) == 1);

            input_cells_idxs.erase(
                std::lower_bound(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx));

            lue_assert(std::count(input_cells_idxs.begin(), input_cells_idxs.end(), input_cell_idx) == 0);
        }

        /*!
            @brief      Update the value associated with the partition
                        output cell with index @a output_cell_idxs
            @param      output_cell_idxs Indices of partition output
                        cell. Information for this output cell must be
                        present in the instance.
            @param      value New value
        */
        void update_value(
            Indices const output_cell_idxs,
            Value&& value)
        {
            lue_assert(has_value(output_cell_idxs));

            _values.at(output_cell_idxs) = std::move(value);
        }

        void accumulate_value(
            Indices const output_cell_idxs,
            Value&& value)
        {
            lue_assert(has_value(output_cell_idxs));

            _values.at(output_cell_idxs) += std::move(value);
        }

        CellsIdxs const& input_cells(
            Indices const output_cell_idxs) const
        {
            lue_assert(has_input_cells(output_cell_idxs));

            return _input_cells_idxs.at(output_cell_idxs);
        }

        Value const& value(
            Indices const output_cell_idxs) const
        {
            lue_assert(has_value(output_cell_idxs));

            return _values.at(output_cell_idxs);
        }

        CellsIdxsMap const& input_cells_idxs() const
        {
            return _input_cells_idxs;
        }

        ValueMap const& values() const
        {
            return _values;
        }

    private:

        friend class hpx::serialization::access;

        void serialize(
            hpx::serialization::input_archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            archive & _input_cells_idxs & _values;

            // assert_invariants();
        }

        void serialize(
            hpx::serialization::output_archive& archive,
            [[maybe_unused]] unsigned int const version) const
        {
            // assert_invariants();

            archive & _input_cells_idxs & _values;
        }

        bool has_input_cells(
            Indices const output_cell_idxs) const
        {
            return _input_cells_idxs.find(output_cell_idxs) != _input_cells_idxs.end();
        }

        bool has_value(
            Indices const output_cell_idxs) const
        {
            return _values.find(output_cell_idxs) != _values.end();
        }

        CellsIdxs& input_cells_idxs(
            Indices const output_cell_idxs)
        {
            lue_assert(has_input_cells(output_cell_idxs));

            return _input_cells_idxs.at(output_cell_idxs);
        }

        //! Map of collection of sorted input cell indices by output cell index
        CellsIdxsMap _input_cells_idxs;

        //! Map of value instances per output cell index
        ValueMap _values;

};

}  // namespace lue
