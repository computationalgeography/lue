#include "lue/framework/algorithm/value_policies/reclassify.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename FromElement, typename ToElement, Rank rank>
        auto reclassify2(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table) -> PartitionedArray<ToElement, rank>
        {
            return value_policies::reclassify(array, lookup_table);
        }


        template<typename ToElement2, typename FromElement, typename ToElement1>
        auto cast_lut(LookupTable<FromElement, ToElement1> const& lookup_table)
            -> LookupTable<FromElement, ToElement2>
        {
            static_assert(std::is_integral_v<FromElement>);
            static_assert(std::is_floating_point_v<ToElement1>);
            static_assert(std::is_floating_point_v<ToElement2>);

            if constexpr (std::is_same_v<ToElement1, ToElement2>)
            {
                return lookup_table;
            }
            else
            {
                LookupTable<FromElement, ToElement2> result;

                for (auto const& [key, value] : lookup_table)
                {
                    result[key] = static_cast<ToElement2>(value);
                }

                return result;
            }
        }


        template<typename FromElement, typename ToElement, Rank rank>
        auto reclassify(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table,
            pybind11::dtype const& dtype) -> pybind11::object
        {
            // Switch on dtype and call a function that returns an array of the
            // right value type
            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            break;
                        }
                        case 8:
                        {
                            break;
                        }
                    }

                    break;
                }
                case 'u':
                {
                    // Unsigned integer
                    switch (size)
                    {
                        case 1:
                        {
                            break;
                        }
                        case 4:
                        {
                            break;
                        }
                        case 8:
                        {
                            break;
                        }
                    }

                    break;
                }
                case 'f':
                {
                    // Floating-point
                    switch (size)
                    {
                        case 4:
                        {
                            using Element = float;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(reclassify2(array, cast_lut<Element>(lookup_table)));
                            }

                            break;
                        }
                        case 8:
                        {
                            using Element = double;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(reclassify2(array, cast_lut<Element>(lookup_table)));
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }


        // template<typename FromElement, typename ToElement, Rank rank>
        // auto reclassify(
        //     PartitionedArray<FromElement, rank> const& array,
        //     LookupTable<FromElement, ToElement> const& lookup_table,
        //     pybind11::object const& dtype_args) -> pybind11::object
        // {
        //     return reclassify1(array, lookup_table, pybind11::dtype::from_args(dtype_args));
        // }


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};
                    using FromElement = Element;
                    using ToElement = LargestFloatingPointElement;

                    module.def(
                        "reclassify",
                        [](PartitionedArray<FromElement, rank> const& array,
                           LookupTable<FromElement, ToElement> const& lookup_table,
                           pybind11::object const& dtype_args)  // -> pybind11::object
                        { return reclassify(array, lookup_table, pybind11::dtype::from_args(dtype_args)); },
                        "array"_a,
                        "lookup_table"_a,
                        "dtype"_a);
                }
        };

    }  // Anonymous namespace


    void bind_reclassify(pybind11::module& module)
    {
        // The lookup table is a Python dictionary. The dtype argument is to allow the caller
        // to specify exactly what type to use for the resulting array.

        bind<Binder, UnsignedIntegralElements>(module);
    }

}  // namespace lue::framework
