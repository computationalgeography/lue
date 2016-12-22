#pragma once
#include "lue/cxx_api/hdf5/file.h"
#include "lue/cxx_api/phenomena.h"
#include "lue/cxx_api/universes.h"
#include <memory>
#include <string>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
    @brief      This class is for managing LUE datasets.

    A dataset represents everything that a LUE file contains. Datasets
    contain collections of:
    - universes, representing alternative states of a system
    - phenomena, representing one state of a system
*/
class Dataset:
    public hdf5::File
{

public:

                   Dataset             (hdf5::Identifier&& location);

                   Dataset             (Dataset const& other)=delete;

                   Dataset             (Dataset&& other)=default;

                   ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const& other)=delete;

    Dataset&       operator=           (Dataset&& other)=default;

    Universe&      add_universe        (std::string const& name);

    Phenomenon&    add_phenomenon      (std::string const& name);

    Universes&     universes           () const;

    Phenomena&     phenomena           () const;

private:

    std::unique_ptr<Universes> _universes;

    std::unique_ptr<Phenomena> _phenomena;

};


Dataset            open_dataset        (std::string const& name,
                                        unsigned int const flags);

Dataset            create_dataset      (std::string const& name);

} // namespace lue
