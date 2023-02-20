#include "lue/object/dataset.hpp"
#include "lue/configure.hpp"
#include <cassert>
#include <stdexcept>


namespace lue {
    namespace data_model {

        Dataset::Dataset(
            std::string const& name,
            unsigned int const flags,
            AccessPropertyList const& access_property_list):

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

            hdf5::File{std::forward<hdf5::File>(file)},
            _description{
                attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""},
            _universes{*this},
            _phenomena{*this}

        {
        }


        std::string Dataset::lue_version() const
        {
            return attribute<std::string>("lue_version");
        }


        /*!
            @brief      Add new universe to dataset
            @sa         Universes::add()
        */
        Universe& Dataset::add_universe(std::string const& name)
        {
            return _universes.add(name);
        }


        /*!
            @brief      Add new phenomenon to dataset
            @sa         Phenomena::add()
        */
        Phenomenon& Dataset::add_phenomenon(std::string const& name, std::string const& description)
        {
            return _phenomena.add(name, description);
        }


        std::string const& Dataset::description() const
        {
            return _description;
        }


        /*!
            @brief      Return universes collection.
        */
        Universes const& Dataset::universes() const
        {
            return _universes;
        }


        /*!
            @brief      Return universes collection.
        */
        Universes& Dataset::universes()
        {
            return _universes;
        }


        /*!
            @brief      Return phenomena collection
        */
        Phenomena const& Dataset::phenomena() const
        {
            return _phenomena;
        }


        /*!
            @brief      Return phenomena collection
        */
        Phenomena& Dataset::phenomena()
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
        bool dataset_exists(std::string const& name)
        {
            return hdf5::file_exists(name);
        }


        namespace {

            Dataset create_dataset(hdf5::File&& file, std::string const& description)
            {
                create_universes(file);
                create_phenomena(file);

                file.attributes().write<std::string>("lue_version", LUE_VERSION);

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
        Dataset open_dataset(
            std::string const& name,
            unsigned int flags,
            hdf5::File::AccessPropertyList const& access_property_list)
        {
            return Dataset{name, flags, access_property_list};
        }


        /*!
            @brief      Create dataset
            @param      name Name of dataset
            @return     Dataset instance
            @exception  std::runtime_error In case the dataset cannot be created
        */
        Dataset create_dataset(
            std::string const& name,
            std::string const& description,
            hdf5::File::AccessPropertyList access_property_list)
        {
            access_property_list.set_library_version_bounds(::H5F_LIBVER_LATEST, ::H5F_LIBVER_LATEST);

            hdf5::File file{hdf5::create_file(name, std::move(access_property_list))};

            return create_dataset(std::move(file), description);
        }


        Dataset create_in_memory_dataset(std::string const& name, std::string const& description)
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

    }  // namespace data_model
}  // namespace lue
