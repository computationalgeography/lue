#include "lue/framework/algorithm/value_policies/add.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_add_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("add", [](Array const& array1, Array const& array2) { return add(array1, array2); });
            module.def("add", [](Array const& array, Scalar const& scalar) { return add(array, scalar); });
            module.def("add", [](Array const& array, Value const value) { return add(array, value); });

            module.def("add", [](Scalar const& scalar, Array const& array) { return add(scalar, array); });
            module.def(
                "add", [](Scalar const& scalar1, Scalar const& scalar2) { return add(scalar1, scalar2); });
            module.def("add", [](Scalar const& scalar, Value const value) { return add(scalar, value); });

            module.def("add", [](Value const value, Array const& array) { return add(value, array); });
            module.def("add", [](Value const value, Scalar const& scalar) { return add(value, scalar); });

            module.def("add", [](Value const value1, Value const value2) { return add(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_add(pybind11::module& module)
    {
        define_add_overloads<std::uint8_t>(module);
        define_add_overloads<std::uint32_t>(module);
        define_add_overloads<std::uint64_t>(module);
        define_add_overloads<std::int32_t>(module);
        define_add_overloads<std::int64_t>(module);
        define_add_overloads<float>(module);
        define_add_overloads<double>(module);
    }

}  // namespace lue::framework
