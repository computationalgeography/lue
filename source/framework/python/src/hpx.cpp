#include "lue/framework/partitioned_array.hpp"
#include "lue/py/framework/stream.hpp"
#include <hpx/config.hpp>
#include <hpx/future.hpp>
#include <fmt/format.h>
#include <pybind11/stl.h>


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
        static std::string formal_string_representation(
            [[maybe_unused]] hpx::shared_future<Element> const& future)
        {
            return fmt::format("shared_future<{}>", as_string<Element>());
        }


        template<typename Element>
        static std::string informal_string_representation(hpx::shared_future<Element> const& future)
        {
            return formal_string_representation(future);
        }


        template<typename Element>
        void bind_future(pybind11::module& module)
        {
            pybind11::class_<hpx::future<Element>>(
                module, fmt::format("future<{}>", as_string<Element>()).c_str())

                .def("get", [](hpx::future<Element>& future) { return future.get(); })

                .def("wait", [](hpx::future<Element> const& future) { return future.wait(); })

                ;
        }


        template<typename Element>
        void bind_shared_future(pybind11::module& module)
        {
            pybind11::class_<hpx::shared_future<Element>>(
                module, fmt::format("shared_future<{}>", as_string<Element>()).c_str())

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

                .def("wait", [](hpx::shared_future<Element> const& future) { return future.wait(); })

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

    }  // Anonymous namespace


    void bind_hpx(pybind11::module& module)
    {
        bind_future<void>(module);

        // bind_future<std::uint8_t>(module);
        // bind_future<std::uint32_t>(module);
        // bind_future<std::uint64_t>(module);
        // bind_future<std::int32_t>(module);
        // bind_future<std::int64_t>(module);
        // bind_future<float>(module);
        // bind_future<double>(module);

        bind_shared_future<std::uint8_t>(module);
        bind_shared_future<std::uint32_t>(module);
        bind_shared_future<std::uint64_t>(module);
        bind_shared_future<std::int32_t>(module);
        bind_shared_future<std::int64_t>(module);
        bind_shared_future<float>(module);
        bind_shared_future<double>(module);

        bind_shared_future<std::vector<PartitionedArray<std::uint8_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<std::uint32_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<std::uint64_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<std::int32_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<std::int64_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<float, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<double, 2>>>(module);

        bind_shared_future<std::map<std::uint8_t, std::vector<float>>>(module);
        bind_shared_future<std::map<std::uint32_t, std::vector<float>>>(module);
        bind_shared_future<std::map<std::uint64_t, std::vector<float>>>(module);
        bind_shared_future<std::map<std::int32_t, std::vector<float>>>(module);
        bind_shared_future<std::map<std::int64_t, std::vector<float>>>(module);

        bind_shared_future<std::map<std::uint8_t, std::vector<double>>>(module);
        bind_shared_future<std::map<std::uint32_t, std::vector<double>>>(module);
        bind_shared_future<std::map<std::uint64_t, std::vector<double>>>(module);
        bind_shared_future<std::map<std::int32_t, std::vector<double>>>(module);
        bind_shared_future<std::map<std::int64_t, std::vector<double>>>(module);

        pybind11::class_<hpx::shared_future<void>>(
            module, fmt::format("shared_future<{}>", as_string<void>()).c_str())

            .def(pybind11::init<hpx::shared_future<uint8_t>>())
            .def(pybind11::init<hpx::shared_future<uint32_t>>())
            .def(pybind11::init<hpx::shared_future<uint64_t>>())
            .def(pybind11::init<hpx::shared_future<int32_t>>())
            .def(pybind11::init<hpx::shared_future<int64_t>>())
            .def(pybind11::init<hpx::shared_future<float>>())
            .def(pybind11::init<hpx::shared_future<double>>())
            .def(pybind11::init([](pybind11::none const&) { return hpx::make_ready_future<void>(); }))

            .def(
                "__repr__",
                [](hpx::shared_future<void> const& future) { return formal_string_representation(future); })

            .def(
                "__str__",
                [](hpx::shared_future<void> const& future) { return informal_string_representation(future); })

            .def("wait", [](hpx::shared_future<void> const& future) { return future.wait(); })

            ;

        pybind11::implicitly_convertible<pybind11::none, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<uint8_t>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<uint32_t>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<uint64_t>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<int32_t>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<int64_t>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<float>, hpx::shared_future<void>>();
        pybind11::implicitly_convertible<hpx::shared_future<double>, hpx::shared_future<void>>();

        // bind_make_ready_future<std::map<std::uint8_t, std::vector<float>>>(module);
        // bind_make_ready_future<std::map<std::uint32_t, std::vector<float>>>(module);
        // bind_make_ready_future<std::map<std::uint64_t, std::vector<float>>>(module);
        bind_make_ready_future<std::map<std::int32_t, std::vector<float>>>(module);
        // bind_make_ready_future<std::map<std::int64_t, std::vector<float>>>(module);

        // bind_make_ready_future<std::map<std::uint8_t, std::vector<double>>>(module);
        // bind_make_ready_future<std::map<std::uint32_t, std::vector<double>>>(module);
        // bind_make_ready_future<std::map<std::uint64_t, std::vector<double>>>(module);
        // bind_make_ready_future<std::map<std::int32_t, std::vector<double>>>(module);
        // bind_make_ready_future<std::map<std::int64_t, std::vector<double>>>(module);
    }

}  // namespace lue::framework
