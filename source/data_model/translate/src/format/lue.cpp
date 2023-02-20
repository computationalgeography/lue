#include "lue/translate/format/lue.hpp"


namespace lue {
    namespace utility {

        /*!
            @brief      Try to open a dataset read-only
            @return     A pointer to a Dataset instance if the dataset can be
                        opened. Otherwise a default constructed pointer.
        */
        LUEDatasetPtr try_open_lue_dataset_for_read(std::string const& dataset_name)
        {
            LUEDatasetPtr result;

            try
            {
                result = std::make_unique<data_model::Dataset>(dataset_name, H5F_ACC_RDONLY);
            }
            catch (...)
            {
            }

            return result;
        }

    }  // namespace utility
}  // namespace lue
