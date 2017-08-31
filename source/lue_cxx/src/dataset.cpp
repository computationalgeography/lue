#include "lue/dataset.hpp"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @brief      Open dataset
    @param      name Name of dataset
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @exception  std::runtime_error In case the dataset cannot be opened

    It is assumed that the dataset already exists.
*/
Dataset::Dataset(
    std::string const& name,
    unsigned int const flags)

    : hdf5::File(name, flags),
      _universes(id()),
      _phenomena(id())

{
}


/*!
    @brief      Construct an instance based on an HDF5 identfier
                representing the dataset

    If universes and phenomena are present in the collections, they will
    be opened.
*/
Dataset::Dataset(
    hdf5::Identifier&& location)

    : hdf5::File(std::forward<hdf5::Identifier>(location)),
      _universes(id()),
      _phenomena(id())

{
}


Dataset::Dataset(
    hdf5::File&& file)

    : hdf5::File(std::forward<hdf5::File>(file)),
      _universes(id()),
      _phenomena(id())

{
}


/*!
    @brief      Add new universe to dataset
    @sa         Universes::add()
*/
Universe& Dataset::add_universe(
    std::string const& name)
{
    return _universes.add(name);
}


/*!
    @brief      Add new phenomenon to dataset
    @sa         Phenomena::add()
*/
Phenomenon& Dataset::add_phenomenon(
    std::string const& name)
{
    return _phenomena.add(name);
}


/*!
    @brief      return universes collection.
    @warning    a universes collection must be present
*/
Universes const& Dataset::universes() const
{
    return _universes;
}


/*!
    @brief      return universes collection.
    @warning    a universes collection must be present
*/
Universes& Dataset::universes()
{
    return _universes;
}


/*!
    @brief      Return phenomena collection
    @warning    A phenomena collection must be present
*/
Phenomena const& Dataset::phenomena() const
{
    return _phenomena;
}


/*!
    @brief      Return phenomena collection
    @warning    A phenomena collection must be present
*/
Phenomena& Dataset::phenomena()
{
    return _phenomena;
}


/*
    @brief      Open dataset
    @param      name Name of dataset
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @return     Dataset instance
    @exception  std::runtime_error In case the dataset cannot be opened
Dataset open_dataset(
    std::string const& name,
    unsigned int const flags)
{
    hdf5::Identifier dataset_location(::open_dataset(name.c_str(),
        flags), ::close_dataset);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Dataset " + name + " cannot be opened");
    }

    return Dataset(std::move(dataset_location));
}
*/


/*!
    @brief      Return whether or not a dataset exists
    @param      name Name of dataset
    @warning    This function only checks whether a regular file named
                @a name is present. No attempt is made to verify the file
                is accessible or is formatted correctly.
*/
bool dataset_exists(
    std::string const& name)
{
    return hdf5::file_exists(name);
}


/*!
    @brief      Create dataset
    @param      name Name of dataset
    @return     Dataset instance
    @exception  std::runtime_error In case the dataset cannot be created
*/
Dataset create_dataset(
    std::string const& name)
{
    auto file = hdf5::create_file(name);

    create_universes(file.id());
    create_phenomena(file.id());

    return Dataset(std::move(file));
}


/*!
    @brief      Remove dataset
    @param      name Name of dataset
    @exception  std::runtime_error In case the dataset cannot be removed
*/
void remove_dataset(
    std::string const& name)
{
    hdf5::remove_file(name);
}

} // namespace lue
