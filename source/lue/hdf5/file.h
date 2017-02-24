#pragma once
#include "lue/hdf5/group.h"


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an HDF5 file.
*/
class File:
    public Group
{

public:

                   File                (std::string const& name,
                                        unsigned int const flags);

                   File                (Identifier&& id);

                   File                (File const& other)=delete;

                   File                (File&& other);

    virtual        ~File               ()=default;

    File&          operator=           (File const& other)=delete;

    File&          operator=           (File&& other);

    std::string    pathname            () const;

private:

};


// File               open_file           (std::string const& name,
//                                         unsigned int const flags);

bool               file_exists         (std::string const& name);

File               create_file         (std::string const& name);

void               remove_file         (std::string const& name);

} // namespace hdf5
} // namespace lue
