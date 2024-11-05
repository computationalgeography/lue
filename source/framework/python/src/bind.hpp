#pragma once
#include "lue/concept.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    template<typename Binder, Arithmetic Element>
    void bind(pybind11::module& module)
    {
        Binder::template bind<Element>(module);
    }


    template<typename Binder, TupleLike Elements, std::size_t idx>
    void bind(pybind11::module& module) requires(idx == 0)
    {
        bind<Binder, std::tuple_element_t<idx, Elements>>(module);
    }


    template<typename Binder, TupleLike Elements, std::size_t idx>
    void bind(pybind11::module& module) requires(idx > 0)
    {
        bind<Binder, std::tuple_element_t<idx, Elements>>(module);
        bind<Binder, Elements, idx - 1>(module);
    }


    template<typename Binder, TupleLike Elements>
    void bind(pybind11::module& module)
    {
        bind<Binder, Elements, std::tuple_size_v<Elements> - 1>(module);
    }


    template<typename Binder, Arithmetic Element1, Arithmetic Element2>
    void bind(pybind11::module& module)
    {
        Binder::template bind<Element1, Element2>(module);
    }


    template<typename Binder, Arithmetic Element1, TupleLike Elements2, std::size_t idx>
    void bind(pybind11::module& module) requires(idx == 0)
    {
        bind<Binder, Element1, std::tuple_element_t<idx, Elements2>>(module);
    }


    template<typename Binder, Arithmetic Element1, TupleLike Elements2, std::size_t idx>
    void bind(pybind11::module& module) requires(idx > 0)
    {
        bind<Binder, Element1, std::tuple_element_t<idx, Elements2>>(module);
        bind<Binder, Element1, Elements2, idx - 1>(module);
    }


    template<typename Binder, TupleLike Elements1, std::size_t idx, TupleLike Elements2>
    void bind(pybind11::module& module) requires(idx == 0)
    {
        bind<Binder, std::tuple_element_t<idx, Elements1>, Elements2, std::tuple_size_v<Elements2> - 1>(
            module);
    }


    template<typename Binder, TupleLike Elements1, std::size_t idx, TupleLike Elements2>
    void bind(pybind11::module& module) requires(idx > 0)
    {
        bind<Binder, std::tuple_element_t<idx, Elements1>, Elements2, std::tuple_size_v<Elements2> - 1>(
            module);
        bind<Binder, Elements1, idx - 1, Elements2>(module);
    }


    template<typename Binder, TupleLike Elements1, TupleLike Elements2>
    void bind(pybind11::module& module)
    {
        bind<Binder, Elements1, std::tuple_size_v<Elements1> - 1, Elements2>(module);
    }

}  // namespace lue::framework
