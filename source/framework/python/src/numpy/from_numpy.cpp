#include "bind.hpp"
#include "shape.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>  // std::optional


using namespace pybind11::literals;


namespace lue::framework {

    namespace {

        template<typename T>
        using Array = pybind11::array_t<T>;

        template<typename T>
        using Object = std::tuple<Array<T>, pybind11::buffer_info>;

        template<typename T>
        using ReferenceCountedObject = std::shared_ptr<Object<T>>;

    }  // Anonymous namespace

}  // namespace lue::framework


namespace lue::detail {

    // template<
    //     typename T>
    // class ArrayTraits<
    //     pybind11::array_t<T>>
    // {

    //     public:

    //         using Element = T;

    // };

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
        pybind11::object from_numpy_py(
            pybind11::array_t<Element> const& array,
            std::optional<pybind11::tuple> const& partition_shape,
            std::optional<Element> const& no_data_value)
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
            using Functor = lue::InstantiateFromBuffer<ReferenceCountedObject<Element>, 2>;

            ReferenceCountedObject<Element> object{
                std::make_shared<Object<Element>>(std::make_tuple(array, array.request()))};

            return pybind11::cast(lue::create_partitioned_array(
                Policies{},
                static_array_shape,
                static_partition_shape,
                Functor{
                    object,  // Copy: increments reference count
                    [](ReferenceCountedObject<Element> const& object) -> Element*
                    { return static_cast<Element*>(std::get<1>(*object).ptr); },
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
