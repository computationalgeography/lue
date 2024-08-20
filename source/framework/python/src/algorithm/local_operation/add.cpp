#include "lue/py/framework/algorithm/add.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_add_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "add", add<Element, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);
            module.def("add", add<Element, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("add", add<Element, rank, PartitionedArray<Element, rank>, Element>);

            module.def("add", add<Element, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("add", add<Element, rank, Element, PartitionedArray<Element, rank>>);

            module.def("add", add<Element, Scalar<Element>, Element>);
            module.def("add", add<Element, Element, Scalar<Element>>);
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
