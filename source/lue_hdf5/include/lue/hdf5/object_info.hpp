#pragma once
#include <hdf5.h>


namespace lue {
namespace hdf5 {

class ObjectInfo
{

public:

    explicit       ObjectInfo          (hid_t id);

                   ObjectInfo          (ObjectInfo const&)=delete;

                   ObjectInfo          (ObjectInfo&&)=default;

                   ~ObjectInfo         ()=default;

    ObjectInfo&    operator=           (ObjectInfo const&)=delete;

    ObjectInfo&    operator=           (ObjectInfo&&)=default;

    bool           operator==          (ObjectInfo const& other) const;

    bool           operator!=          (ObjectInfo const& other) const;

    unsigned long  fileno              () const;

    haddr_t        addr                () const;

private:

    H5O_info_t     _info;

};

}  // namespace hdf5
}  // namespace lue
