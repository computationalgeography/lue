#pragma once
#include "lue/hdf5/file.hpp"
#include "lue/object/phenomena.hpp"
#include "lue/object/universes.hpp"


namespace lue {

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
                                        unsigned int const
                                            flags=H5F_ACC_RDONLY);

                   Dataset             (hdf5::File&& file);

                   Dataset             (Dataset const&)=delete;

                   Dataset             (Dataset&&)=default;

                   ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const&)=delete;

    Dataset&       operator=           (Dataset&&)=default;

    Universe&      add_universe        (std::string const& name);

    Phenomenon&    add_phenomenon      (std::string const& name);

    Universes const&
                   universes           () const;

    Universes&     universes           ();

    Phenomena const&
                   phenomena           () const;

    Phenomena&     phenomena           ();

private:

    Universes      _universes;

    Phenomena      _phenomena;

};


bool               dataset_exists      (std::string const& name);

Dataset            create_dataset      (std::string const& name);

void               remove_dataset      (std::string const& name);

} // namespace lue
