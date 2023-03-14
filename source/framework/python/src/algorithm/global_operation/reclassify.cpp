#include "lue/framework/algorithm/value_policies/reclassify.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename FromElement, typename ToElement, Rank rank>
        PartitionedArray<ToElement, rank> reclassify2(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table)
        {
            return value_policies::reclassify(array, lookup_table);
        }


        template<typename ToElement2, typename FromElement, typename ToElement1>
        LookupTable<FromElement, ToElement2> cast_lut(
            LookupTable<FromElement, ToElement1> const& lookup_table)
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
        pybind11::object reclassify1(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table,
            pybind11::dtype const& dtype)
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
                            result = pybind11::cast(reclassify2(array, cast_lut<float>(lookup_table)));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(reclassify2(array, cast_lut<double>(lookup_table)));
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


        template<typename FromElement, typename ToElement, Rank rank>
        pybind11::object reclassify(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table,
            pybind11::object const& dtype_args)
        {
            return reclassify1(array, lookup_table, pybind11::dtype::from_args(dtype_args));
        }

    }  // Anonymous namespace


    void bind_reclassify(pybind11::module& module)
    {
        // The lookup table is a Python dictionary. The dtype argument is to allow the caller
        // to specify exactly what type to use for the resulting array.
        module.def("reclassify", reclassify<std::uint8_t, double, 2>, "array"_a, "lookup_table"_a, "dtype"_a);
        module.def(
            "reclassify", reclassify<std::uint32_t, double, 2>, "array"_a, "lookup_table"_a, "dtype"_a);
        module.def("reclassify", reclassify<std::int32_t, double, 2>, "array"_a, "lookup_table"_a, "dtype"_a);
        module.def(
            "reclassify", reclassify<std::uint64_t, double, 2>, "array"_a, "lookup_table"_a, "dtype"_a);
        module.def("reclassify", reclassify<std::int64_t, double, 2>, "array"_a, "lookup_table"_a, "dtype"_a);
    }

}  // namespace lue::framework
