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

                auto operator=(Dataset const&) -> Dataset& = default;

                auto operator=(Dataset&&) -> Dataset& = default;

                auto lue_version() const -> std::string;

                auto add_universe(std::string const& name) -> Universe&;

                auto add_phenomenon(std::string const& name, std::string const& description = "")
                    -> Phenomenon&;

                auto description() const -> std::string const&;

                auto universes() const -> Universes const&;

                auto universes() -> Universes&;

                auto phenomena() const -> Phenomena const&;

                auto phenomena() -> Phenomena&;

            private:

                std::string _description;

                Universes _universes;

                Phenomena _phenomena;
        };


        LUE_DATA_MODEL_EXPORT auto dataset_exists(std::string const& name) -> bool;

        LUE_DATA_MODEL_EXPORT auto open_dataset(
            std::string const& name,
            unsigned int flags = H5F_ACC_RDONLY,
            hdf5::File::AccessPropertyList const& access_property_list = hdf5::File::AccessPropertyList{})
            -> Dataset;

        LUE_DATA_MODEL_EXPORT auto create_dataset(
            std::string const& name,
            std::string const& description = "",
            hdf5::File::AccessPropertyList access_property_list = hdf5::File::AccessPropertyList{})
            -> Dataset;

        LUE_DATA_MODEL_EXPORT auto create_in_memory_dataset(
            std::string const& name, std::string const& description = "") -> Dataset;

        LUE_DATA_MODEL_EXPORT void remove_dataset(std::string const& name);

    }  // namespace data_model
}  // namespace lue
