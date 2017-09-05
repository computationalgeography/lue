#pragma once
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/property_list.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      This class represents an HDF5 file
*/
class File:
    public Group
{

public:

    class AccessPropertyList:
        public PropertyList
    {

    public:

                   AccessPropertyList  ();

        void       use_core_driver     ();

    };

                   File                (std::string const& name);

                   File                (std::string const& name,
                                        unsigned int const flags);

                   File                (std::string const& name,
                                        unsigned int const flags,
                                        AccessPropertyList const&
                                            access_property_list);

                   File                (Identifier&& id);

                   File                (File const& other)=delete;

                   File                (File&& other);

    virtual        ~File               ()=default;

    File&          operator=           (File const& other)=delete;

    File&          operator=           (File&& other);

    std::string    pathname            () const;

    void           flush               () const;

private:

};


// File               open_file           (std::string const& name,
//                                         unsigned int const flags);

bool               file_exists         (std::string const& name);

File               create_file         (std::string const& name);

File               create_in_memory_file(
                                        std::string const& name);

void               remove_file         (std::string const& name);

} // namespace hdf5
} // namespace lue
