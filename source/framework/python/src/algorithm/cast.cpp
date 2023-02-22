#include "lue/framework/algorithm/value_policies/cast.hpp"
#include <pybind11/numpy.h>


namespace lue::framework {
    namespace {

        template<typename OutputElement, typename InputElement, Rank rank>
        PartitionedArray<OutputElement, rank> cast(PartitionedArray<InputElement, rank> const& array)
        {
            return value_policies::cast<OutputElement>(array);
        }


        template<typename InputElement, Rank rank>
        pybind11::object cast(
            PartitionedArray<InputElement, rank> const& array, pybind11::object const& dtype_args)
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

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
                            result = pybind11::cast(cast<float, InputElement>(array));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(cast<double, InputElement>(array));
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

    }  // Anonymous namespace


    void bind_cast(pybind11::module& module)
    {
        module.def("cast", cast<std::uint64_t, 2>);
        module.def("cast", cast<std::int64_t, 2>);
    }

}  // namespace lue::framework
