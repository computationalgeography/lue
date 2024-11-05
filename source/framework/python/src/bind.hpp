#pragma once
#include "lue/concept.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    template<typename Binder, TupleLike Elements, std::size_t idx>
    void bind(pybind11::module& module) requires(idx == 0)
    {
        Binder::template bind<std::tuple_element_t<idx, Elements>>(module);
    }


    template<typename Binder, TupleLike Elements, std::size_t idx>
    void bind(pybind11::module& module) requires(idx > 0)
    {
        Binder::template bind<std::tuple_element_t<idx, Elements>>(module);
        bind<Binder, Elements, idx - 1>(module);
    }


    template<typename Binder, TupleLike Elements>
    void bind(pybind11::module& module)
    {
        bind<Binder, Elements, std::tuple_size_v<Elements> - 1>(module);
    }

}  // namespace lue::framework
