#include "shape.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>  // std::optional


using namespace pybind11::literals;


namespace lue::framework {

    namespace {

        template<typename T>
        using ReferenceCountedObject = std::shared_ptr<T[]>;

    }  // Anonymous namespace

}  // namespace lue::framework


namespace lue::detail {

    template<typename T>
    class ArrayTraits<lue::framework::ReferenceCountedObject<T>>
    {

        public:

            using Element = T;
    };

}  // namespace lue::detail


namespace lue::framework {

    namespace {

        template<typename Element>
        auto from_numpy_py(
            pybind11::array_t<Element, pybind11::array::c_style> const& array,
            std::optional<pybind11::tuple> const& partition_shape,
            std::optional<Element> const& no_data_value) -> pybind11::object
        {
            // NOTE: For now we assume
            // - All arrays have rank 2
            // - All array elements are in row-major ordering

            Rank const rank(array.ndim());

            if (rank != 2)
            {
                throw std::runtime_error("Currently only arrays of rank 2 are supported");
            }

            StaticShape<2> static_array_shape{};
            std::copy_n(array.shape(), rank, static_array_shape.begin());

            StaticShape<2> static_partition_shape{};

            if (partition_shape)
            {
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (static_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(fmt::format(
                        "Rank of array shape and partition shape must be equal ({} != {})",
                        static_array_shape.size(),
                        dynamic_partition_shape.size()));
                }

                static_partition_shape = dynamic_shape_to_static_shape<2>(dynamic_partition_shape);
            }
            else
            {
                static_partition_shape = default_partition_shape(static_array_shape);
            }

            // The goal is to create a partitioned array, given a Numpy array. We want to do this
            // asynchronously. So, even though the function returns an array immediately, the partitions might
            // not be ready to use yet.
            //
            // This is nice, because other code can already attach continuations to the partitions. Or do
            // something else entirely.
            //
            // The problem is that we can't easily use the Python API in threads. This requires obtaining and
            // releasing the GIL from/to the thread running the Python interpreter.
            // A solution for this is to just copy the data here, and then asynchronously create the
            // partitions

            using Policies = lue::policy::create_partitioned_array::DefaultValuePolicies<Element>;
            using Functor = lue::InstantiateFromBuffer<ReferenceCountedObject<Element>, 2>;

            // The source array buffer
            pybind11::buffer_info source_array_buffer_info{array.request()};
            Element const* source_buffer{static_cast<Element*>(source_array_buffer_info.ptr)};

            // The target array buffer
            auto const nr_elements(lue::nr_elements(static_array_shape));
            ReferenceCountedObject<Element> copy_buffer(std::make_shared<Element[]>(nr_elements));
            std::copy(source_buffer, source_buffer + nr_elements, copy_buffer.get());

            return pybind11::cast(lue::create_partitioned_array(
                Policies{},
                static_array_shape,
                static_partition_shape,
                Functor{
                    std::move(copy_buffer),
                    [](ReferenceCountedObject<Element> const& buffer) -> Element* { return buffer.get(); },
                    no_data_value}));
        }


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    // TODO Create a single wrapper function, accepting a py::array and work from
                    //      there. Document this one wrapper function, etc.
                    module.def(
                        "from_numpy",
                        from_numpy_py<Element>,
                        "array"_a.noconvert(),
                        pybind11::kw_only(),
                        "partition_shape"_a = std::optional<pybind11::tuple>{},
                        "no_data_value"_a = std::optional<Element>{});
                }
        };

    }  // Anonymous namespace


    void bind_from_numpy(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
