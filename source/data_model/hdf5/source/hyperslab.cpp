#include "lue/hdf5/hyperslab.hpp"
#include <algorithm>
#include <cassert>
#include <utility>


namespace lue::hdf5 {

    /*!
        @brief      Construct an instance based on a @a start, @a stride,
                    and @a count

        The size of @a start, @a stride, and @a count must be equal to the rank
        of the dataset's dataspace.
    */
    Hyperslab::Hyperslab(Offset start, Stride stride, Count count):

        _start{std::move(start)},
        _stride{std::move(stride)},
        _count{std::move(count)}

    {
        assert(_start.size() == _stride.size());
        assert(_start.size() == _count.size());
    }


    /*!
        @brief      Construct an instance based on a @a start, and @a count

        - The stride is initialized to 1 in all directions

        The size of @a start, and @a count must be equal to the rank
        of the dataset's dataspace.
    */
    Hyperslab::Hyperslab(Offset start, Count count):

        _start{std::move(start)},
        _stride(_start.size(), 1),
        _count{std::move(count)}

    {
        assert(_start.size() == _count.size());
    }


    /*!
        @brief      Construct an instance based on a @a shape

        The instance represents the whole dataspace of the @a shape passed in:

        - The offset is initialized to 0 in all directions
        - The stride is initialized to 1 in all directions
        - The count is initialized to the values in @a shape

        The size of @a shape must be equal to the rank of the dataset's dataspace.
    */
    Hyperslab::Hyperslab(Shape const& shape):

        _start(shape.size(), 0),
        _stride(shape.size(), 1),
        _count(shape.begin(), shape.end())

    {
        assert(_start.size() == _stride.size());
        assert(_start.size() == _count.size());
    }


    /*!
        @brief      Return the offset in each dimension of the dataspace selection
    */
    auto Hyperslab::start() const -> Offset const&
    {
        return _start;
    }


    /*!
        @brief      Return the offset in each dimension of the dataspace selection
    */
    auto Hyperslab::start() -> Offset&
    {
        return _start;
    }


    /*!
        @brief      Return the stride of the blocks in each dimension of the dataspace selection
    */
    auto Hyperslab::stride() const -> Stride const&
    {
        return _stride;
    }


    /*!
        @brief      Return the stride of the blocks in each dimension of the dataspace selection
    */
    auto Hyperslab::stride() -> Stride&
    {
        return _stride;
    }


    /*!
        @brief      Return the number of blocks in each dimension of the dataspace selection
    */
    auto Hyperslab::count() const -> Count const&
    {
        return _count;
    }


    /*!
        @brief      Return the number of blocks in each dimension of the dataspace selection
    */
    auto Hyperslab::count() -> Count&
    {
        return _count;
    }


    /*!
        @brief      Return the number of dimensions of the dataspace selection
    */
    auto Hyperslab::nr_dimensions() const -> std::size_t
    {
        return _start.size();
    }


    /*!
        @brief      Return the dataspace selection is empty

        This happens in case the number of dimensions is zero or one of the counts or strides is 0.
    */
    auto Hyperslab::empty() const -> bool
    {
        return nr_dimensions() == 0 || std::ranges::find(_count, 0) != _count.end() ||
               std::ranges::find(_stride, 0) != _stride.end();
    }


    /*!
        @brief      Return the number of selected elements
    */
    auto Hyperslab::nr_elements() const -> std::size_t
    {
        std::size_t result = 0;

        if (!empty())
        {
            result = 1;

            for (std::size_t i = 0; i < nr_dimensions(); ++i)
            {
                result *= (_stride[i] * _count[i]);
            }
        }

        return result;
    }


    auto hyperslab(Offset const& center, Shape const& shape, [[maybe_unused]] Shape const& array_shape)
        -> Hyperslab
    {
        assert(array_shape.size() == 2);
        assert(center.size() == array_shape.size());
        assert(shape.size() == array_shape.size());

        assert(center[0] >= shape[0] / 2 && center[1] >= shape[1] / 2);

        Offset const start{center[0] - (shape[0] / 2), center[1] - (shape[1] / 2)};
        Count const count{shape[0], shape[1]};

        Hyperslab const hyperslab{start, count};

        // Assert shape of hyperslab equals shape passed in:
        assert(hyperslab.count()[0] == shape[0]);
        assert(hyperslab.count()[1] == shape[1]);

        // Assert hyperslab is positioned within the array:
        assert(
            (hyperslab.start()[0] + hyperslab.count()[0] <= array_shape[0]) &&
            (hyperslab.start()[1] + hyperslab.count()[1] <= array_shape[1]));

        return hyperslab;
    }

}  // namespace lue::hdf5
