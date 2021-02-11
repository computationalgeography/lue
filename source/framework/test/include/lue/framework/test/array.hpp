#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"


namespace lue {

    template<
        typename Array>
    class Test
    {
    };


    template<
        typename Element>
    class Test<PartitionedArray<Element, 1>>
    {

        public:

            using Array = PartitionedArray<Element, 1>;
            using Shape = ShapeT<Array>;
            using Index = typename Shape::value_type;

            static constexpr Shape const& shape()
            {
                return _array_shape;
            }

            static constexpr Shape const& partition_shape()
            {
                return _partition_shape;
            }

        private:

            static constexpr Shape _array_shape{{100}};

            static constexpr Shape _partition_shape{{10}};

    };


    template<
        typename Element>
    class Test<PartitionedArray<Element, 2>>
    {

        public:

            using Array = PartitionedArray<Element, 2>;
            using Shape = ShapeT<Array>;
            using Index = typename Shape::value_type;

            static constexpr Shape const& shape()
            {
                return _array_shape;
            }

            static constexpr Shape const& partition_shape()
            {
                return _partition_shape;
            }

        private:

            static constexpr Shape _array_shape{{60, 40}};

            static constexpr Shape _partition_shape{{10, 10}};

    };


    namespace test {

        template<
            typename Array,
            typename Shape>
        Array create_partitioned_array(
            Shape const& array_shape,
            Shape const& partition_shape,
            std::initializer_list<std::initializer_list<ElementT<Array>>> elements)
        {
            using Partition = PartitionT<Array>;
            using Data = DataT<Partition>;

            Array array{lue::create_partitioned_array<ElementT<Array>>(array_shape, partition_shape)};

            lue_assert(static_cast<Count>(elements.size()) == array.nr_partitions());

            {
                auto elements_it = elements.begin();

                for(Partition& partition: array.partitions())
                {
                    partition.wait();  // FIXME be made asynchronous
                    partition.set_data(Data{partition.shape().get(), *elements_it});
                    ++elements_it;
                }
            }

            return array;
        }

    }  // namespace test
}  // namespace lue
