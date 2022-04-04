#include "shape.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include <pybind11/numpy.h>
#include <fmt/format.h>


namespace lue::framework {

    namespace {

        template<
            typename T>
        using Array = pybind11::array_t<T>;

        template<
            typename T>
        using Object = std::tuple<Array<T>, pybind11::buffer_info>;

        template<
            typename T>
        using ReferenceCountedObject = std::shared_ptr<Object<T>>;

    }  // Anonymous namespace

}


namespace lue::detail {

    // template<
    //     typename T>
    // class ArrayTraits<
    //     pybind11::array_t<T>>
    // {

    //     public:

    //         using Element = T;

    // };

    template<
        typename T>
    class ArrayTraits<
        lue::framework::ReferenceCountedObject<T>>
    {

        public:

            using Element = T;

    };

}  // namespace lue::detail


namespace lue::framework {

    namespace {

        // TODO Handle partition shape checks, in main operation

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> from_numpy3(
            pybind11::array_t<Element> const& array,
            StaticShape<rank> const& partition_shape)
        {
            // The goal is to create a partitioned array, given a Numpy array. We want to do
            // this asynchronously. So, even though the function returns an array immediately,
            // the partitions might not be ready to use yet.
            // This is nice, because other code can already attach continuations to the
            // partitions. Or do something else entirely.
            // This is a bit dangerous as well:
            // - Modifying the Numpy array while partitions are still being created
            //   results in rubish in the resulting array. Don't do this.
            // - The Numpy array must not be deleted while array partitions are still being
            //   created. This is prevented by copying array instances around. This increases
            //   the reference count of the underlying pybind11 object.

            // The reference counted object is a shared pointer of:
            // - The array passed in
            // - The pybind11::buffer_info instance of the array
            // This second thing provides us with a pointer to the buffer array elements. Obtaining
            // it should only be done once, for all partitions. Therefore we do that here and glue
            // it to the array. A shared pointer to this tuple is passed around in the create_array
            // function. We don't use the reference counting of the array itself, apart from
            // the one copy we make when we create the tuple.

            using Policies = lue::policy::create_partitioned_array::DefaultValuePolicies<Element>;
            using Functor = lue::InstantiateFromBuffer<ReferenceCountedObject<Element>, rank>;

            StaticShape<rank> array_shape{};
            std::copy_n(array.shape(), rank, array_shape.begin());

            // TODO How to handle no-data?

            ReferenceCountedObject<Element> object{
                    std::make_shared<Object<Element>>(
                        std::make_tuple(array, array.request()))
                };

            return lue::create_partitioned_array(
                    Policies{}, array_shape, partition_shape,
                    Functor{object,  // Copy: increments reference count
                            [](ReferenceCountedObject<Element> const& object) -> Element*
                            {
                                return static_cast<Element*>(std::get<1>(*object).ptr);
                            }
                        }
                );
        }


        template<
            typename Element>
        pybind11::object from_numpy2(
            pybind11::array_t<Element> const& array,
            DynamicShape const& partition_shape)
        {
            if(DynamicShape::size_type(array.ndim()) != partition_shape.size())
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and partition shape must be equal ({} != {})",
                    array.ndim(), partition_shape.size()));
            }

            // NOTE: For now we assume
            // - All arrays have rank 2
            // - All array elements are in row-major ordering

            Rank const rank(array.ndim());
            pybind11::object result;

            if(rank == 2)
            {
                result = pybind11::cast(from_numpy3<Element, 2>(
                    array, dynamic_shape_to_static_shape<2>(partition_shape)));
            }
            else
            {
                throw std::runtime_error("Currently only arrays of rank 2 are supported");
            }

            lue_hpx_assert(result);

            return result;
        }


        template<
            typename Element>
        pybind11::object from_numpy(
            pybind11::array_t<Element> const& array,
            pybind11::tuple const& partition_shape)
        {
            return from_numpy2(array, tuple_to_shape(partition_shape));
        }

    }  // Anonymous namespace


    void bind_from_numpy(
        pybind11::module& module)
    {
        module.def("from_numpy", from_numpy<uint8_t>);
        module.def("from_numpy", from_numpy<uint32_t>);
        module.def("from_numpy", from_numpy<uint64_t>);
        module.def("from_numpy", from_numpy<int32_t>);
        module.def("from_numpy", from_numpy<int64_t>);
        module.def("from_numpy", from_numpy<float>);
        module.def("from_numpy", from_numpy<double>);
    }

}  // namespace lue::framework
