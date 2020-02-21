#pragma once
#include <hpx/config.hpp>
#include <lue/data_model/hl.hpp>
#include <hpx/include/lcos.hpp>
#include <string>


namespace lue {

template<
    typename Array>
Array              read        (Array const& array,
                                data_model::variable::RasterView::Layer const& layer,
                                data_model::Index idx);

}  // namespace lue
