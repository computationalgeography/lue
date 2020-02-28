#pragma once
#include <hpx/config.hpp>
#include <lue/data_model/hl.hpp>
#include <hpx/include/lcos.hpp>
#include <string>


namespace lue {

template<
    typename Array>
[[nodiscard]] hpx::future<void>
                   write               (Array const& array,
                                        std::string const& pathname);

template<
    typename Array,
    typename DatasetPtr=std::shared_ptr<data_model::Dataset>>
[[nodiscard]] hpx::future<void>
                   write               (
        Array const& array,
        typename data_model::constant::RasterView<DatasetPtr>::Layer& layer);

template<
    typename Array,
    typename DatasetPtr=std::shared_ptr<data_model::Dataset>>
[[nodiscard]] hpx::future<void>
                   write               (
        Array const& array,
        typename data_model::variable::RasterView<DatasetPtr>::Layer& layer,
        data_model::Index idx);

}  // namespace lue
