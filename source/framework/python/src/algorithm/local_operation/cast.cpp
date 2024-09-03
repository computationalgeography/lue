#include "lue/framework/algorithm/value_policies/cast.hpp"
#include <pybind11/numpy.h>


namespace lue::framework {
    namespace {

        template<typename Argument>
        auto cast(Argument const& argument, pybind11::object const& dtype_args) -> pybind11::object
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
                            result = pybind11::cast(value_policies::cast<std::int32_t>(argument));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::cast<std::int64_t>(argument));
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
                            result = pybind11::cast(value_policies::cast<std::uint8_t>(argument));
                            break;
                        }
                        case 4:
                        {
                            result = pybind11::cast(value_policies::cast<std::uint32_t>(argument));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::cast<std::uint64_t>(argument));
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
                            result = pybind11::cast(value_policies::cast<float>(argument));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::cast<double>(argument));
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


        template<typename Element>
        void bind_cast_overloads(pybind11::module& module)
        {
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "cast",
                [](Array const& array, pybind11::object const& dtype_args)
                { return cast(array, dtype_args); });
            module.def(
                "cast",
                [](Scalar const& scalar, pybind11::object const& dtype_args)
                { return cast(scalar, dtype_args); });
            module.def(
                "cast",
                [](Value const value, pybind11::object const& dtype_args)
                { return cast(value, dtype_args); });
        }

    }  // Anonymous namespace


    void bind_cast(pybind11::module& module)
    {
        bind_cast_overloads<std::uint8_t>(module);
        bind_cast_overloads<std::uint32_t>(module);
        bind_cast_overloads<std::int32_t>(module);
        bind_cast_overloads<std::uint64_t>(module);
        bind_cast_overloads<std::int64_t>(module);
        bind_cast_overloads<float>(module);
        bind_cast_overloads<double>(module);
    }

}  // namespace lue::framework
