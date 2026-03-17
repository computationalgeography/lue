#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include "lue/py/framework/stream.hpp"
#include <hpx/config.hpp>
#include <hpx/future.hpp>
#include <pybind11/stl.h>
#include <format>


namespace lue::framework {

    template<>
    class TypeTraits<std::vector<PartitionedArray<std::uint8_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::uint8_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::uint32_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::uint32_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::uint64_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::uint64_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::int8_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::int8_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::int32_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::int32_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::int64_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::int64_t, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<float, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<float, 2>>"};
    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<double, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<double, 2>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint8_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint8_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint32_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint32_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint64_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint64_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::int8_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::int8_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::int32_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::int32_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::int64_t, std::vector<float>>>
    {

        public:

            inline static std::string const name{"std::map<std::int64_t, std::vector<float>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint8_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint8_t, std::vector<double>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint32_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint32_t, std::vector<double>>"};
    };


    template<>
    class TypeTraits<std::map<std::uint64_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::uint64_t, std::vector<double>>"};
    };


    template<>
    class TypeTraits<std::map<std::int8_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::int8_t, std::vector<double>>"};
    };


    template<>
    class TypeTraits<std::map<std::int32_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::int32_t, std::vector<double>>"};
    };


    template<>
    class TypeTraits<std::map<std::int64_t, std::vector<double>>>
    {

        public:

            inline static std::string const name{"std::map<std::int64_t, std::vector<double>>"};
    };


    namespace {

        template<typename Element>
        auto formal_string_representation([[maybe_unused]] hpx::shared_future<Element> const& future)
            -> std::string
        {
            return std::format("shared_future<{}>", as_string<Element>());
        }


        template<typename Element>
        auto informal_string_representation(hpx::shared_future<Element> const& future) -> std::string
        {
            return formal_string_representation(future);
        }


        template<typename Element>
        void bind_future(pybind11::module& module)
        {
            pybind11::class_<hpx::future<Element>>(
                module, std::format("future<{}>", as_string<Element>()).c_str())

                .def("get", [](hpx::future<Element>& future) { return future.get(); })

                ;
        }


        template<typename Element>
        void bind_shared_future(pybind11::module& module)
        {
            pybind11::class_<hpx::shared_future<Element>>(
                module, std::format("shared_future<{}>", as_string<Element>()).c_str())

                .def(
                    "__repr__",
                    [](hpx::shared_future<Element> const& future)
                    { return formal_string_representation(future); })

                .def(
                    "__str__",
                    [](hpx::shared_future<Element> const& future)
                    { return informal_string_representation(future); })

                .def(
                    "get",
                    pybind11::overload_cast<>(&hpx::shared_future<Element>::get, pybind11::const_),
                    // Copy to keep things simple. Use const reference in case Element can be big.
                    pybind11::return_value_policy::move)

                ;
        }


        template<typename T>
        auto make_ready_future(T const& value) -> hpx::shared_future<T>
        {
            return hpx::make_ready_future<T>(value).share();
        }


        template<typename T>
        void bind_make_ready_future(pybind11::module& module)
        {
            module.def(
                "make_ready_future",
                make_ready_future<T>,
                pybind11::arg("value"),
                pybind11::return_value_policy::move);
        }


        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            bind_shared_future<Element>(module);
            bind_shared_future<std::vector<PartitionedArray<Element, 2>>>(module);
            pybind11::implicitly_convertible<hpx::shared_future<Element>, hpx::shared_future<void>>();
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module)
            requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module)
            requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }


        template<typename Element, typename FloatingPointElement>
        void bind(pybind11::module& module)
        {
            bind_shared_future<std::map<Element, std::vector<FloatingPointElement>>>(module);
        }


        template<typename Element, TupleLike FloatingPointElements, std::size_t idx>
        void bind(pybind11::module& module)
            requires(idx == 0)
        {
            bind<Element, std::tuple_element_t<idx, FloatingPointElements>>(module);
        }


        template<typename Element, TupleLike FloatingPointElements, std::size_t idx>
        void bind(pybind11::module& module)
            requires(idx > 0)
        {
            bind<Element, std::tuple_element_t<idx, FloatingPointElements>>(module);
            bind<Element, FloatingPointElements, idx - 1>(module);
        }


        template<TupleLike Elements, std::size_t idx, TupleLike FloatingPointElements>
        void bind(pybind11::module& module)
            requires(idx == 0)
        {
            bind<
                std::tuple_element_t<idx, Elements>,
                FloatingPointElements,
                std::tuple_size_v<FloatingPointElements> - 1>(module);
        }


        template<TupleLike Elements, std::size_t idx, TupleLike FloatingPointElements>
        void bind(pybind11::module& module)
            requires(idx > 0)
        {
            bind<
                std::tuple_element_t<idx, Elements>,
                FloatingPointElements,
                std::tuple_size_v<FloatingPointElements> - 1>(module);
            bind<Elements, idx - 1, FloatingPointElements>(module);
        }


        template<TupleLike Elements, TupleLike FloatingPointElements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1, FloatingPointElements>(module);
        }


        template<typename Element>
        void bind(pybind11::class_<hpx::shared_future<void>>& class_)
        {
            class_.def(pybind11::init<hpx::shared_future<Element>>());
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::class_<hpx::shared_future<void>>& class_)
            requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(class_);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::class_<hpx::shared_future<void>>& class_)
            requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(class_);
            bind<Elements, idx - 1>(class_);
        }


        template<TupleLike Elements>
        void bind(pybind11::class_<hpx::shared_future<void>>& class_)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(class_);
        }

    }  // Anonymous namespace


    void bind_hpx(pybind11::module& module)
    {
        bind_future<void>(module);

        auto shared_future_class =
            pybind11::class_<hpx::shared_future<void>>(
                module, std::format("shared_future<{}>", as_string<void>()).c_str())
                .def(pybind11::init([](pybind11::none const&) { return hpx::make_ready_future<void>(); }))

                .def(
                    "__repr__",
                    [](hpx::shared_future<void> const& future)
                    { return formal_string_representation(future); })

                .def(
                    "__str__",
                    [](hpx::shared_future<void> const& future)
                    { return informal_string_representation(future); })

                .def("get", [](hpx::shared_future<void>& future) { return future.get(); })

            ;

        // TODO Use bind<> from bind.hpp, and a Binder

        bind<ArithmeticElements>(module);
        bind<IntegralElements, FloatingPointElements>(module);

        pybind11::implicitly_convertible<pybind11::none, hpx::shared_future<void>>();
        bind<ArithmeticElements>(shared_future_class);

        // // These are required by image land.
        // // TODO: Refactor and only bind for types we need.
        // bind_make_ready_future<std::map<std::uint8_t, std::vector<float>>>(module);
        // bind_make_ready_future<std::map<std::uint8_t, std::vector<double>>>(module);

        // // bind_make_ready_future<std::map<std::int8_t, std::vector<float>>>(module);
        // // bind_make_ready_future<std::map<std::int8_t, std::vector<double>>>(module);

        // // bind_make_ready_future<std::map<std::uint32_t, std::vector<float>>>(module);
        // // bind_make_ready_future<std::map<std::uint32_t, std::vector<double>>>(module);

        // bind_make_ready_future<std::map<std::int32_t, std::vector<float>>>(module);
        // bind_make_ready_future<std::map<std::int32_t, std::vector<double>>>(module);

        // // bind_make_ready_future<std::map<std::uint64_t, std::vector<float>>>(module);
        // // bind_make_ready_future<std::map<std::uint64_t, std::vector<double>>>(module);

        // // bind_make_ready_future<std::map<std::int64_t, std::vector<float>>>(module);
        // // bind_make_ready_future<std::map<std::int64_t, std::vector<double>>>(module);
    }

}  // namespace lue::framework
