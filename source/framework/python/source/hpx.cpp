#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include "lue/py/framework/stream.hpp"
#include <hpx/config.hpp>
#include <hpx/future.hpp>
#include <pybind11/stl.h>
#include <format>


namespace lue::framework {

    template<Arithmetic Element>
    class TypeTraits<std::vector<PartitionedArray<Element, 2>>>
    {

        public:

            inline static std::string const name{
                std::format("std::vector<lue::PartitionedArray<{}, 2>>", as_string<Element>())};
    };


    template<std::integral IntegralElement, std::floating_point FloatingPointElement>
    class TypeTraits<std::map<IntegralElement, std::vector<FloatingPointElement>>>
    {

        public:

            inline static std::string const name{std::format(
                "std::map<{}, std::vector<{}>>",
                as_string<IntegralElement>(),
                as_string<FloatingPointElement>())};
    };


    namespace {

        template<typename T>
        auto formal_string_representation([[maybe_unused]] hpx::shared_future<T> const& future) -> std::string
        {
            return std::format("shared_future<{}>", as_string<T>());
        }


        template<typename T>
        auto informal_string_representation(hpx::shared_future<T> const& future) -> std::string
        {
            return formal_string_representation(future);
        }


        template<typename T>
        void bind_shared_future(pybind11::module& module)
        {
            pybind11::class_<hpx::shared_future<T>>(
                module, std::format("shared_future<{}>", as_string<T>()).c_str())

                .def(
                    "__repr__",
                    [](hpx::shared_future<T> const& future) -> std::string
                    { return formal_string_representation(future); })

                .def(
                    "__str__",
                    [](hpx::shared_future<T> const& future) -> std::string
                    { return informal_string_representation(future); })

                .def(
                    "get",
                    pybind11::overload_cast<>(&hpx::shared_future<T>::get, pybind11::const_),
                    pybind11::return_value_policy::move)

                ;
        }


        class SharedFutureBinder1
        {
            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    bind_shared_future<Element>(module);
                    bind_shared_future<std::vector<PartitionedArray<Element, rank>>>(module);
                }

            private:
        };


        class SharedFutureBinder2
        {
            public:

                template<std::integral IntegralElement, std::floating_point FloatingPointElement>
                static void bind(pybind11::module& module)
                {
                    bind_shared_future<std::map<IntegralElement, std::vector<FloatingPointElement>>>(module);
                }

            private:
        };

    }  // namespace


    void bind_hpx(pybind11::module& module)
    {
        auto const shared_future_void =
            pybind11::class_<hpx::shared_future<void>>(module, "shared_future<void>");
        pybind11::setattr(module, "State", shared_future_void);

        pybind11::class_<hpx::future<void>>(module, "future<void>")
            .def("get", pybind11::overload_cast<>(&hpx::future<void>::get));


        bind<SharedFutureBinder1, ArithmeticElements>(module);
        bind<SharedFutureBinder2, IntegralElements, FloatingPointElements>(module);

        module.def(
            "as_state",
            [](pybind11::none const&) -> hpx::shared_future<void> { return hpx::make_ready_future(); });
    }

}  // Namespace lue::framework
