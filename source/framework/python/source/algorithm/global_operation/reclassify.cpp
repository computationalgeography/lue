#include "lue/framework/algorithm/value_policies/reclassify.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename ToElement, std::integral FromElement, std::floating_point LUTElement>
        auto cast_lut(LookupTable<FromElement, LUTElement> const& lookup_table)
            -> LookupTable<FromElement, ToElement>
        {
            if constexpr (std::is_same_v<LUTElement, ToElement>)
            {
                return lookup_table;
            }
            else
            {
                LookupTable<FromElement, ToElement> result;

                for (auto const& [key, value] : lookup_table)
                {
                    result[key] = static_cast<ToElement>(value);
                }

                return result;
            }
        }


        template<Arithmetic ToElement, std::integral FromElement, std::floating_point LUTElement, Rank rank>
        auto reclassify(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, LUTElement> const& lookup_table) -> pybind11::object
        {
            pybind11::object result{};

            if constexpr (arithmetic_element_supported<ToElement>)
            {
                result = pybind11::cast(value_policies::reclassify(array, cast_lut<ToElement>(lookup_table)));
            }

            return result;
        }


        template<std::integral FromElement, std::floating_point LUTElement, Rank rank>
        auto reclassify(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, LUTElement> const& lookup_table,
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
                        case 1:
                        {
                            result = reclassify<std::int8_t>(array, lookup_table);
                            break;
                        }
                        case 4:
                        {
                            result = reclassify<std::int32_t>(array, lookup_table);
                            break;
                        }
                        case 8:
                        {
                            result = reclassify<std::int64_t>(array, lookup_table);
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
                            result = reclassify<std::uint8_t>(array, lookup_table);
                            break;
                        }
                        case 4:
                        {
                            result = reclassify<std::uint32_t>(array, lookup_table);
                            break;
                        }
                        case 8:
                        {
                            result = reclassify<std::uint64_t>(array, lookup_table);
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
                            result = reclassify<float>(array, lookup_table);
                            break;
                        }
                        case 8:
                        {
                            result = reclassify<double>(array, lookup_table);
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


        class Binder
        {

            public:

                template<std::integral Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};
                    using FromElement = Element;
                    using LUTElement = LargestFloatingPointElement;

                    module.def(
                        "reclassify",
                        [](PartitionedArray<FromElement, rank> const& array,
                           LookupTable<FromElement, LUTElement> const& lookup_table,
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

        bind<Binder, IntegralElements>(module);
    }

}  // namespace lue::framework
