#include "lue/framework/algorithm/value_policies/abs.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("abs", [](Array const& array) { return abs(array); });
            module.def("abs", [](Scalar const& scalar) { return abs(scalar); });
            module.def("abs", [](Value const value) { return abs(value); });
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }

    }  // Anonymous namespace

    void bind_abs(pybind11::module& module)
    {
        bind<SignedIntegralElements>(module);
        bind<FloatingPointElements>(module);
    }

}  // namespace lue::framework
