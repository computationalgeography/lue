#pragma once
#include <hdf5.h>


namespace lue {
namespace hdf5 {

class ObjectInfo
{

public:

    explicit       ObjectInfo          (hid_t id);

                   ObjectInfo          (ObjectInfo const& other)=delete;

                   ObjectInfo          (ObjectInfo&& other)=default;

                   ~ObjectInfo         ()=default;

    ObjectInfo&    operator=           (ObjectInfo const& other)=delete;

    ObjectInfo&    operator=           (ObjectInfo&& other)=default;

    unsigned long  fileno              () const;

    haddr_t        addr                () const;

private:

    H5O_info_t     _info;

};


bool               operator==          (ObjectInfo const& lhs,
                                        ObjectInfo const& rhs);

bool               operator!=          (ObjectInfo const& lhs,
                                        ObjectInfo const& rhs);

}  // namespace hdf5
}  // namespace lue
