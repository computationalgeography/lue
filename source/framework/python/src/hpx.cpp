#include <hpx/config.hpp>
#include "lue/py/framework/stream.hpp"
#include "lue/framework/partitioned_array.hpp"
#include <hpx/future.hpp>
#include <pybind11/stl.h>
#include <fmt/format.h>


namespace lue::framework {

    template<>
    class TypeTraits<std::vector<PartitionedArray<std::uint8_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::uint8_t, 2>"};

    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::uint32_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::uint32_t, 2>"};

    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::int32_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::int32_t, 2>"};

    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<std::int64_t, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<std::int64_t, 2>"};

    };


    template<>
    class TypeTraits<std::vector<PartitionedArray<double, 2>>>
    {

        public:

            inline static std::string const name{"std::vector<lue::PartitionedArray<double, 2>"};

    };


    namespace {

        template<
            typename Element>
        static std::string formal_string_representation(
            [[maybe_unused]] hpx::shared_future<Element> const& future)
        {
            return fmt::format(
                "shared_future<{}>",
                as_string<Element>());
        }


        template<
            typename Element>
        static std::string informal_string_representation(
            hpx::shared_future<Element> const& future)
        {
            return formal_string_representation(future);
        }


        template<
            typename Element>
        void bind_future(
            pybind11::module& module)
        {
            pybind11::class_<hpx::future<Element>>(
                module,
                fmt::format("future<{}>", as_string<Element>()).c_str())

                .def(
                    "get",
                    [](hpx::future<Element>& future) {
                        return future.get();
                    })

                .def(
                    "wait",
                    [](hpx::future<Element> const& future) {
                        return future.wait();
                    })

                ;
        }


        template<
            typename Element>
        void bind_shared_future(
            pybind11::module& module)
        {
            pybind11::class_<hpx::shared_future<Element>>(
                module,
                fmt::format("shared_future<{}>", as_string<Element>()).c_str())

                .def(
                    "__repr__",
                    [](hpx::shared_future<Element> const& future) {
                        return formal_string_representation(future);
                    }
                )

                .def(
                    "__str__",
                    [](hpx::shared_future<Element> const& future) {
                        return informal_string_representation(future);
                    }
                )

                .def(
                    "get",
                    pybind11::overload_cast<>(&hpx::shared_future<Element>::get, pybind11::const_),
                    // Copy to keep things simple. Use const reference in case Element can be big.
                    pybind11::return_value_policy::move)

                .def(
                    "wait",
                    [](hpx::shared_future<Element> const& future) {
                        return future.wait();
                    })

                ;

        }

    }  // Anonymous namespace


    void bind_hpx(
        pybind11::module& module)
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
        bind_shared_future<std::vector<PartitionedArray<std::int32_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<std::int64_t, 2>>>(module);
        bind_shared_future<std::vector<PartitionedArray<double, 2>>>(module);
    }

}  // namespace lue::framework
