#pragma once
#include <hpx/config.hpp>
#include <hpx/include/lcos.hpp>
#include <lue/data_model/hl.hpp>
#include <string>


namespace lue {

    template<typename Array>
    [[nodiscard]] auto write(Array const& array, std::string const& pathname) -> hpx::future<void>;

    template<typename Array, typename DatasetPtr = std::shared_ptr<data_model::Dataset>>
    [[nodiscard]] auto write(
        Array const& array,
        typename data_model::constant::RasterView<DatasetPtr>::Layer& layer) -> hpx::future<void>;

    template<typename Array, typename DatasetPtr = std::shared_ptr<data_model::Dataset>>
    [[nodiscard]] auto write(
        Array const& array,
        typename data_model::variable::RasterView<DatasetPtr>::Layer& layer,
        data_model::Index idx) -> hpx::future<void>;

}  // namespace lue
