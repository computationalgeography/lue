#include "lue/framework/configure.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/concept.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<Arithmetic Element, Rank rank>
        void wait(PartitionedArray<Element, rank> const& array)
        {
            wait_all(array);

            lue_hpx_assert(all_are_valid(array));
            lue_hpx_assert(all_are_ready(array));
        }


        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            module.def("wait", wait<Element, 2>);
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


    void bind_wait_partitioned_array(pybind11::module& module)
    {
        bind<ArithmeticElements>(module);
    }

}  // namespace lue::framework
