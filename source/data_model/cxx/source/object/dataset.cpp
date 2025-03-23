#include "lue/object/dataset.hpp"
#include "lue/version.hpp"
#include <cassert>


namespace lue::data_model {

    Dataset::Dataset(
        std::string const& name, unsigned int const flags, AccessPropertyList const& access_property_list):

        hdf5::File{name, flags, access_property_list},
        _description{
            attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""},
        _universes{*this},
        _phenomena{*this}

    {
    }


    Dataset::Dataset(std::string const& name, AccessPropertyList const& access_property_list):

        Dataset{name, H5F_ACC_RDONLY, access_property_list}

    {
    }


    /*!
        @brief      Open dataset
        @param      name Name of dataset
        @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
        @exception  std::runtime_error In case the dataset cannot be opened

        It is assumed that the dataset already exists.
    */
    Dataset::Dataset(std::string const& name, unsigned int const flags):

        Dataset{name, flags, AccessPropertyList{}}

    {
    }


    Dataset::Dataset(hdf5::File&& file):

        hdf5::File{std::move(file)},
        _description{
            attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""},
        _universes{*this},
        _phenomena{*this}

    {
    }


    auto Dataset::lue_version() const -> std::string
    {
        return attribute<std::string>("lue_version");
    }


    /*!
        @brief      Add new universe to dataset
        @sa         Universes::add()
    */
    auto Dataset::add_universe(std::string const& name) -> Universe&
    {
        return _universes.add(name);
    }


    /*!
        @brief      Add new phenomenon to dataset
        @sa         Phenomena::add()
    */
    auto Dataset::add_phenomenon(std::string const& name, std::string const& description) -> Phenomenon&
    {
        return _phenomena.add(name, description);
    }


    auto Dataset::description() const -> std::string const&
    {
        return _description;
    }


    /*!
        @brief      Return universes collection.
    */
    auto Dataset::universes() const -> Universes const&
    {
        return _universes;
    }


    /*!
        @brief      Return universes collection.
    */
    auto Dataset::universes() -> Universes&
    {
        return _universes;
    }


    /*!
        @brief      Return phenomena collection
    */
    auto Dataset::phenomena() const -> Phenomena const&
    {
        return _phenomena;
    }


    /*!
        @brief      Return phenomena collection
    */
    auto Dataset::phenomena() -> Phenomena&
    {
        return _phenomena;
    }


    /*!
        @brief      Return whether or not a dataset exists
        @param      name Name of dataset
        @warning    This function only checks whether a regular file named
                    @a name is present. No attempt is made to verify the file
                    is accessible or is formatted correctly.
    */
    auto dataset_exists(std::string const& name) -> bool
    {
        return hdf5::file_exists(name);
    }


    namespace {

        auto create_dataset(hdf5::File&& file, std::string const& description) -> Dataset
        {
            create_universes(file);
            create_phenomena(file);

            file.attributes().write<std::string>("lue_version", version());

            if (!description.empty())
            {
                file.attributes().write<std::string>(description_tag, description);
            }

            return Dataset{std::move(file)};
        }

    }  // Anonymous namespace


    /*!
        @brief      Open dataset
        @param      name Name of dataset
        @return     Dataset instance
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto open_dataset(
        std::string const& name,
        unsigned int flags,
        hdf5::File::AccessPropertyList const& access_property_list) -> Dataset
    {
        return Dataset{name, flags, access_property_list};
    }


    /*!
        @brief      Create dataset
        @param      name Name of dataset
        @return     Dataset instance
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto create_dataset(
        std::string const& name,
        std::string const& description,
        hdf5::File::AccessPropertyList access_property_list) -> Dataset
    {
        access_property_list.set_library_version_bounds(::H5F_LIBVER_LATEST, ::H5F_LIBVER_LATEST);

        hdf5::File file{hdf5::create_file(name, std::move(access_property_list))};

        return create_dataset(std::move(file), description);
    }


    auto create_in_memory_dataset(std::string const& name, std::string const& description) -> Dataset
    {
        hdf5::File file{hdf5::create_in_memory_file(name)};

        return create_dataset(std::move(file), description);
    }


    /*!
        @brief      Remove dataset
        @param      name Name of dataset
        @exception  std::runtime_error In case the dataset cannot be removed
    */
    void remove_dataset(std::string const& name)
    {
        hdf5::remove_file(name);
    }

}  // namespace lue::data_model
