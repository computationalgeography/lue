#include "lue/cxx_api/dataset.h"
#include "lue/c_api/dataset.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @brief      Construct an instance based on an HDF5 identfier
                representing the dataset.

    If collections of universes and phenomena are present, they will be
    opened.
*/
Dataset::Dataset(
    hdf5::Identifier&& location)

    : hdf5::File(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    assert(universes_exists(id()));
    _universes = std::make_unique<Universes>(id());

    assert(phenomena_exists(id()));
    _phenomena = std::make_unique<Phenomena>(id());
}


/*!
    @brief      Add new universe to dataset
    @sa         Universes::add()
*/
Universe& Dataset::add_universe(
    std::string const& name)
{
    return universes().add(name);
}


/*!
    @brief      Add new phenomenon to dataset
    @sa         Phenomena::add()
*/
Phenomenon& Dataset::add_phenomenon(
    std::string const& name)
{
    return phenomena().add(name);
}


/*!
    @brief      Return universes collection.
    @warning    A universes collection must be present
*/
Universes& Dataset::universes() const
{
    assert(_universes);

    return *_universes;
}


/*!
    @brief      Return phenomena collection.
    @warning    A phenomena collection must be present
*/
Phenomena& Dataset::phenomena() const
{
    assert(_phenomena);

    return *_phenomena;
}


/*!
    @ingroup    lue_cxx_api_group
    @brief      Open dataset
    @param      name Name of dataset
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @return     Dataset instance
    @exception  std::runtime_error In case the dataset cannot be opened
*/
Dataset open_dataset(
    std::string const& name,
    unsigned int const flags)
{
    hdf5::Identifier dataset_location(::open_dataset(name.c_str(),
        flags), ::close_dataset);
        // H5F_ACC_RDWR), ::close_dataset);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Dataset " + name + " cannot be opened");
    }

    return Dataset(std::move(dataset_location));
}


/*!
    @ingroup    lue_cxx_api_group
    @brief      Create dataset
    @param      name Name of dataset
    @return     Dataset instance
    @exception  std::runtime_error In case the dataset cannot be created
*/
Dataset create_dataset(
    std::string const& name)
{
    hdf5::Identifier dataset_location(::create_dataset(name.c_str()),
        ::close_dataset);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Dataset " + name + " cannot be created");
    }

    return Dataset(std::move(dataset_location));
}

} // namespace lue
