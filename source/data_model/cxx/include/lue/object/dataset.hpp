#pragma once
#include "lue/hdf5/file.hpp"
#include "lue/object/phenomena.hpp"
#include "lue/object/universes.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      This class is for managing LUE datasets.
    @sa         dataset_exists(std::string const&),
                create_dataset(std::string const&),
                remove_dataset(std::string const&)

    A dataset represents everything that a LUE file contains. Datasets
    contain collections of:
    - universes, which together represent alternative states of a system
    - phenomena, which together represent one state of a system
*/
class Dataset:
    public hdf5::File
{

public:

                   Dataset             (std::string const& name,
                                        unsigned int flags,
                                        AccessPropertyList const& access_property_list);

                   Dataset             (std::string const& name,
                                        AccessPropertyList const& access_property_list);

    explicit       Dataset             (std::string const& name,
                                        unsigned int flags=H5F_ACC_RDONLY);

    explicit       Dataset             (hdf5::File&& file);

                   Dataset             (Dataset const&)=default;

                   Dataset             (Dataset&&)=default;

                   ~Dataset            () override =default;

    Dataset&       operator=           (Dataset const&)=default;

    Dataset&       operator=           (Dataset&&)=default;

    std::string    lue_version         () const;

    Universe&      add_universe        (std::string const& name);

    Phenomenon&    add_phenomenon      (std::string const& name,
                                        std::string const& description="");

    std::string const& description     () const;

    Universes const&
                   universes           () const;

    Universes&     universes           ();

    Phenomena const&
                   phenomena           () const;

    Phenomena&     phenomena           ();

private:

    std::string    _description;

    Universes      _universes;

    Phenomena      _phenomena;

};


bool               dataset_exists      (std::string const& name);

Dataset            open_dataset        (std::string const& name,
                                        unsigned int flags=H5F_ACC_RDONLY);

Dataset            create_dataset      (std::string const& name,
                                        std::string const& description="");

Dataset            create_in_memory_dataset(
                                        std::string const& name,
                                        std::string const& description="");

void               remove_dataset      (std::string const& name);

} // namespace data_model
} // namespace lue
