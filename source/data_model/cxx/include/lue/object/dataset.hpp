#pragma once
#include "lue/hdf5/file.hpp"
#include "lue/object/phenomena.hpp"
#include "lue/object/universes.hpp"


namespace lue {
    namespace data_model {

        /*!
            @brief      This class is for managing LUE datasets.

            A dataset represents everything that a LUE file contains. Datasets
            contain collections of:
            - universes, which together represent alternative states of a system
            - phenomena, which together represent one state of a system
        */
        class LUE_DATA_MODEL_EXPORT Dataset: public hdf5::File
        {

            public:

                Dataset(
                    std::string const& name,
                    unsigned int flags,
                    AccessPropertyList const& access_property_list);

                Dataset(std::string const& name, AccessPropertyList const& access_property_list);

                explicit Dataset(std::string const& name, unsigned int flags = H5F_ACC_RDONLY);

                explicit Dataset(hdf5::File&& file);

                Dataset(Dataset const&) = default;

                Dataset(Dataset&&) = default;

                ~Dataset() override = default;

                Dataset& operator=(Dataset const&) = default;

                Dataset& operator=(Dataset&&) = default;

                std::string lue_version() const;

                Universe& add_universe(std::string const& name);

                Phenomenon& add_phenomenon(std::string const& name, std::string const& description = "");

                std::string const& description() const;

                Universes const& universes() const;

                Universes& universes();

                Phenomena const& phenomena() const;

                Phenomena& phenomena();

            private:

                std::string _description;

                Universes _universes;

                Phenomena _phenomena;
        };


        LUE_DATA_MODEL_EXPORT bool dataset_exists(std::string const& name);

        LUE_DATA_MODEL_EXPORT Dataset open_dataset(
            std::string const& name,
            unsigned int flags = H5F_ACC_RDONLY,
            hdf5::File::AccessPropertyList const& access_property_list = hdf5::File::AccessPropertyList{});

        LUE_DATA_MODEL_EXPORT Dataset create_dataset(
            std::string const& name,
            std::string const& description = "",
            hdf5::File::AccessPropertyList access_property_list = hdf5::File::AccessPropertyList{});

        LUE_DATA_MODEL_EXPORT Dataset
        create_in_memory_dataset(std::string const& name, std::string const& description = "");

        LUE_DATA_MODEL_EXPORT void remove_dataset(std::string const& name);

    }  // namespace data_model
}  // namespace lue
