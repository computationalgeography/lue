#pragma once
#include "lue/hdf5/file.hpp"
#include "lue/phenomena.hpp"
#include "lue/universes.hpp"


namespace lue {

/*!
    @brief      This class is for managing LUE datasets.

    A dataset represents everything that a LUE file contains. Datasets
    contain collections of:
    - universes, which together represent alternative states of a system
    - phenomena, which together represent one state of a system

    @sa         dataset_exists(std::string const&),
                create_dataset(std::string const&),
                remove_dataset(std::string const&)
*/
class Dataset:
    public hdf5::File
{

public:

                   Dataset             (std::string const& name,
                                        unsigned int const flags);

                   Dataset             (hdf5::Identifier&& location);

                   Dataset             (hdf5::File&& file);

                   Dataset             (Dataset const& other)=delete;

                   Dataset             (Dataset&& other)=default;

                   ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const& other)=delete;

    Dataset&       operator=           (Dataset&& other)=default;

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


// Dataset            open_dataset        (std::string const& name,
//                                         unsigned int const flags);

bool               dataset_exists      (std::string const& name);

Dataset            create_dataset      (std::string const& name);

void               remove_dataset      (std::string const& name);

} // namespace lue
