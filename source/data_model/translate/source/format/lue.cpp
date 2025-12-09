#include "lue/translate/format/lue.hpp"


namespace lue::utility {

    /*!
        @brief      Try to open a dataset read-only
        @return     A pointer to a Dataset instance if the dataset can be
                    opened. Otherwise a default constructed pointer.
    */
    auto try_open_lue_dataset_for_read(std::string const& dataset_name) -> LUEDatasetPtr
    {
        LUEDatasetPtr result;

        try
        {
            result = std::make_unique<data_model::Dataset>(dataset_name, H5F_ACC_RDONLY);
        }
        // NOLINTBEGIN(bugprone-empty-catch)
        catch (...)
        {
            // Yes, empty catch. Relax, it's fine.
        }
        // NOLINTEND(bugprone-empty-catch)

        return result;
    }

}  // namespace lue::utility
