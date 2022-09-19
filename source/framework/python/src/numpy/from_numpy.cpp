#include "shape.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>  // std::optional
#include <fmt/format.h>


using namespace pybind11::literals;


namespace lue::framework {

    namespace {

        template<
            typename T>
        using Array = pybind11::array_t<T>;

        template<
            typename T>
        using Object = Array<T>;

        template<
            typename T>
        using ReferenceCountedObject = std::shared_ptr<Object<T>>;

    }  // Anonymous namespace

}


namespace lue::detail {

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
            StaticShape<rank> const& partition_shape,
            std::optional<Element> const& no_data_value)
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

            // The reference counted object is a shared pointer to a shallow copy of the array
            // passed in (to keep it alive) The shared pointer to the array is passed around
            // in the create_array function. We don't use the reference counting of the array
            // itself, apart from the one copy we make when we create the shared pointer.

            using Policies = lue::policy::create_partitioned_array::DefaultValuePolicies<Element>;
            using Functor = lue::InstantiateFromBuffer<ReferenceCountedObject<Element>, rank>;

            StaticShape<rank> array_shape{};
            std::copy_n(array.shape(), rank, array_shape.begin());

            ReferenceCountedObject<Element> object{
                    std::make_shared<Object<Element>>(
                            array  // Shallow copy, increments reference count of the array, once
                        )
                };

            // Confirm that the array is shallow copied. Both objects are using the same buffer.
            assert(array.data() == (*object).data());

            return lue::create_partitioned_array(
                    Policies{}, array_shape, partition_shape,
                    Functor{object,  // Copy: increments reference count of the shared pointer
                            [](ReferenceCountedObject<Element> const& object) -> Element const*
                            {
                                return static_cast<Element const*>((*object).data());
                            },
                            no_data_value
                        }
                );
        }


        template<
            typename Element>
        pybind11::object from_numpy2(
            pybind11::array_t<Element> const& array,
            DynamicShape const& partition_shape,
            std::optional<Element> const& no_data_value)
        {
            // TODO https://github.com/computationalgeography/lue/issues/484
            //      In some cases, although a 2D array is passed in, in the code here, array.ndim
            //      returns 1. Throwing the exceptions fails. Segmentation fault, meh.
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
                    array, dynamic_shape_to_static_shape<2>(partition_shape), no_data_value));
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
            pybind11::tuple const& partition_shape,
            std::optional<Element> const& no_data_value)
        {
            return from_numpy2(array, tuple_to_shape(partition_shape), no_data_value);
        }

    }  // Anonymous namespace


    void bind_from_numpy(
        pybind11::module& module)
    {
        module.def("from_numpy", from_numpy<uint8_t>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<uint8_t>{});
        module.def("from_numpy", from_numpy<uint32_t>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<uint32_t>{});
        module.def("from_numpy", from_numpy<uint64_t>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<uint64_t>{});
        module.def("from_numpy", from_numpy<int32_t>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<int32_t>{});
        module.def("from_numpy", from_numpy<int64_t>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<int64_t>{});
        module.def("from_numpy", from_numpy<float>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<float>{});
        module.def("from_numpy", from_numpy<double>,
            "array"_a.noconvert(), "partition_shape"_a, "no_data_value"_a=std::optional<double>{});
    }

}  // namespace lue::framework
