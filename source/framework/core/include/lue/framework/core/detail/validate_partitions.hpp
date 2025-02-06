
/// template<
///     typename Partitions>
/// class ValidateVisitor:
///     public ConstPartitionVisitor<Partitions>
/// {
///
/// private:
///
///     using Base = ConstPartitionVisitor<Partitions>;
///
///     using Partition = PartitionT<Partitions>;
///
///     using Offset = typename Partition::Offset;
///
///     using Shape = ShapeT<Partitions>;
///
///     using Cursor = typename Base::Cursor;
///
/// public:
///
///     ValidateVisitor(
///         Partitions const& partitions,
///         Shape const& shape):
///
///         Base{partitions},
///         _array_shape{shape}
///
///     {
///         _offset.fill(0);
///     }
///
///     void operator()()
///     {
///         // Given a cell, verify it is located within the array's shape, at
///         // the expected offset
///
///         Partition const& partition{this->partition()};
///         partition.get();
///         Offset const partition_offset{partition.offset().get()};
///         _partition_shape = partition.shape().get();
///         Cursor const& cursor{this->cursor()};
///
///         if(partition_offset != _offset) {
///             throw std::runtime_error(fmt::format(
///                 "Partition {} is not located at the expected offset {} "
///                 "in the array but at {}",
///                 cursor.current_cell(), _offset, partition_offset));
///         }
///
///         auto const dimension_idx{cursor.dimension_idx()};
///
///         // Given the current dimension, the next partition should be
///         // offset by the offset of the current partition plus its shape
///         _offset[dimension_idx] += _partition_shape[dimension_idx];
///     }
///
///     void leaving_current_dimension()
///     {
///         // dimension_idx is still pointing to the current (inner) dimension
///         // In case of a raster, cells are visited row-wise.
///
///         // When leaving a dimension, the offset for the inner dimension
///         // + the extent for that dimension of the last seen partition must
///         // match the partitioned array extent. Since we just visited
///         // the last cell in the current dimension, the offset is already
///         // located at the one-past-the-last cell. This value must equal
///         // the extent of the array's shape in the current dimension.
///
///         Cursor const& cursor{this->cursor()};
///         auto const dimension_idx{cursor.dimension_idx()};
///
///         if(_offset[dimension_idx] != _array_shape[dimension_idx])
///         {
///             throw std::runtime_error(fmt::format(
///                 "Partition {} with shape {} does not fit within the array "
///                 "shape {}",
///                 cursor.current_cell(), _partition_shape, _array_shape));
///         }
///
///         // Prepare for leaving the current dimension. In case of a raster,
///         // we must increase the row offset by the number of rows in the
///         // last seen partition and set the column offset to zero (similar
///         // to a carriage return). We will be iterating over the cells
///         // in next row next.
///         lue_hpx_assert(dimension_idx > 0);
///         _offset[dimension_idx - 1] += _partition_shape[dimension_idx - 1];
///         _offset[dimension_idx] = 0;
///     }
///
/// private:
///
///     //! Shape of the array we are validating
///     Shape const    _array_shape;
///
///     //! Offset during visit
///     Offset         _offset;
///
///     //! Shape of last visited partition
///     Shape          _partition_shape;
///
/// };


// WARNING This is an expensive and synchronous operation. Never do
//     this in production code.

/// #ifndef NDEBUG
///
///     // Visit all partitions and see whether they fit exactly within
///     // the shape of the array
///
///     Shape begin_indices;
///     begin_indices.fill(0);
///     Shape const end_indices{_partitions.shape()};
///
///     ValidateVisitor validate{_partitions, _shape};
///
///     visit_array(begin_indices, end_indices, validate);
///
/// #endif
