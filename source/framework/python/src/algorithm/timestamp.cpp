#include "lue/framework/algorithm/timestamp.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "timestamp", [](PartitionedArray<Element, rank> const& array) { return timestamp(array); });
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


    void bind_timestamp(pybind11::module& module)
    {
        static_assert(
            lue::arithmetic_element_supported<ClockTick>,
            "Reconfigure: clock ticks are represented by "
            "std::chrono::high_resolution_clock::duration::rep elements "
            "but LUE is built without support for that type");

        bind<ArithmeticElements>(module);
    }

}  // namespace lue::framework
