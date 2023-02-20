#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"


namespace lue {

    template<typename Array>
    class Test
    {
    };


    template<typename Element>
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


    template<typename Element>
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

        template<typename Array, typename Shape>
        Array create_partitioned_array(
            Shape const& array_shape,
            Shape const& partition_shape,
            std::initializer_list<std::initializer_list<ElementT<Array>>> elements)
        {
            using Partition = PartitionT<Array>;
            using Data = DataT<Partition>;

            Array array{lue::create_partitioned_array<ElementT<Array>>(array_shape, partition_shape)};

            lue_hpx_assert(static_cast<Count>(elements.size()) == array.nr_partitions());

            {
                auto elements_it = elements.begin();

                for (Partition& partition : array.partitions())
                {
                    partition.wait();  // FIXME be made asynchronous
                    partition.set_data(Data{partition.shape().get(), *elements_it});
                    ++elements_it;
                }
            }

            return array;
        }


        template<typename Array>
        Array create_component_array(
            Localities<rank<Array>> const& localities,
            std::initializer_list<DataT<ComponentT<Array>>> elements)
        {
            using Component = ComponentT<Array>;    // HPX component in the array. The elements.
            using Data = DataT<Component>;          // The type of the objects in the initializer list.
            using Components = ComponentsT<Array>;  // The type of the collection of components.

            lue_hpx_assert(nr_elements(localities.shape()) == static_cast<Count>(std::size(elements)));

            Components components{localities.shape()};

            {
                // Fill the array with new components. Each component
                // is initialized with the data passed into this function.

                auto element_it = elements.begin();

                for (std::size_t idx = 0; idx < std::size(elements); ++idx, ++element_it)
                {
                    components[idx] = hpx::new_<Component>(localities[idx], Data{*element_it});
                }
            }

            return Array{localities, std::move(components)};
        }

    }  // namespace test
}  // namespace lue
