#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>  // std::optional
#include <format>


using namespace pybind11::literals;


namespace lue::framework {

    namespace {

        template<typename Element, Rank rank>
        auto to_numpy(
            PartitionedArray<Element, rank> const& array, std::optional<Element> const& no_data_value)
            -> pybind11::array_t<Element>
        {
            // NOTE: For now we assume
            // - All arrays have rank 2
            // - All array elements are in row-major ordering

            if (rank != 2)
            {
                throw std::runtime_error(
                    std::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            auto const shape{lue::shape(array)};

            // Create NumPy array. Ask NumPy to allocate. This happens on the host machine.
            pybind11::array_t<Element> result{pybind11::buffer_info{
                nullptr,
                sizeof(Element),
                pybind11::format_descriptor<Element>::value,
                rank,
                {shape[0], shape[1]},
                {sizeof(Element) * shape[1], sizeof(Element)}}};

            // Copy elements from the partitioned array to the numpy array. This may imply that
            // partitions are downloaded first.
            auto const& partitions{array.partitions()};
            Count const nr_partitions{nr_elements(partitions)};

            pybind11::buffer_info buffer_info{result.request()};
            Element* const result_buffer_ptr_begin{static_cast<Element*>(buffer_info.ptr)};

            lue::policy::DefaultInputNoDataPolicy<Element> indp{};

            for (Index p = 0; p < nr_partitions; ++p)
            {
                partitions[p].get();  // Blocks

                // These calls block and may involve network traffic
                auto partition_data_f{partitions[p].data()};
                auto partition_offset_f{partitions[p].offset()};
                hpx::wait_all(partition_data_f, partition_offset_f);

                auto partition_data{partition_data_f.get()};
                auto const partition_offset{partition_offset_f.get()};

                Element const* source_buffer_ptr{partition_data.data()};
                auto const& partition_shape{partition_data.shape()};
                [[maybe_unused]] auto const nr_partition_elements{nr_elements(partition_shape)};

                Element* result_buffer_ptr =
                    result_buffer_ptr_begin + (partition_offset[0] * shape[1]) + partition_offset[1];

                for (Index idx0 = 0; idx0 < partition_shape[0]; ++idx0)
                {
                    std::copy(source_buffer_ptr, source_buffer_ptr + partition_shape[1], result_buffer_ptr);

                    if (no_data_value)
                    {
                        for (Index i = 0; i < partition_shape[1]; ++i)
                        {
                            if (indp.is_no_data(result_buffer_ptr, i))
                            {
                                result_buffer_ptr[i] = *no_data_value;
                            }
                        }
                    }

                    source_buffer_ptr += partition_shape[1];
                    result_buffer_ptr += shape[1];
                }

                lue_hpx_assert(source_buffer_ptr == partition_data.data() + nr_partition_elements);
            }

            return result;
        }


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "to_numpy",
                        to_numpy<Element, rank>,
                        "array"_a,
                        "no_data_value"_a = std::optional<Element>{});
                }
        };

    }  // Anonymous namespace


    void bind_to_numpy(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
