#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/configure.hpp"
#include "lue/concept.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "array_partition_id",
                [](PartitionedArray<Element, rank> const& array)
                { return array_partition_id<IDElement>(array); });
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


    void bind_array_partition_id(pybind11::module& module)
    {
        bind<ArithmeticElements>(module);
    }

}  // namespace lue::framework
