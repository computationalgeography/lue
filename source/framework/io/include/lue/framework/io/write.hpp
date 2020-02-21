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
    typename Array>
[[nodiscard]] hpx::future<void>
                   write               (
                                Array const& array,
                                data_model::variable::RasterView::Layer& layer,
                                data_model::Index idx);

}  // namespace lue
