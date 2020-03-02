#pragma once
#include <hpx/config.hpp>
#include <lue/data_model/hl.hpp>
#include <hpx/include/lcos.hpp>
#include <string>


namespace lue {

template<
    typename Array,
    typename DatasetPtr=std::shared_ptr<data_model::Dataset>>
Array              read                (Array const& array,
    typename data_model::variable::RasterView<DatasetPtr>::Layer const& layer,
                                        data_model::Index idx);

}  // namespace lue
