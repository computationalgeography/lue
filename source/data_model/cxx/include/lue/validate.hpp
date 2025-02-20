#pragma once
#include "lue/hdf5/file.hpp"
#include "lue/hdf5/issues.hpp"
#include "lue/object/dataset.hpp"


namespace lue {
    namespace data_model {

        LUE_DATA_MODEL_EXPORT std::string message(hdf5::Issues const& issues);

        LUE_DATA_MODEL_EXPORT void validate(Dataset const& dataset, hdf5::Issues& issues);

        LUE_DATA_MODEL_EXPORT void validate(hdf5::File const& file, hdf5::Issues& issues);

        LUE_DATA_MODEL_EXPORT void validate(std::string const& pathname, hdf5::Issues& issues);

        LUE_DATA_MODEL_EXPORT void assert_is_valid(Dataset const& dataset, bool fail_on_warning = true);

        LUE_DATA_MODEL_EXPORT void assert_is_valid(hdf5::File const& file, bool fail_on_warning = true);

        LUE_DATA_MODEL_EXPORT void assert_is_valid(std::string const& name, bool fail_on_warning = true);

    }  // namespace data_model
}  // namespace lue
