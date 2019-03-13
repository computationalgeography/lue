#include "lue/hdf5/soft_link.hpp"
#include "lue/hdf5/group.hpp"
#include <fmt/format.h>


namespace lue {
namespace hdf5 {

SoftLink::SoftLink(
    Group& group,
    std::string const& name):

    Link{group, name}

{
    assert(soft_link_exists(group, name));
}


std::string SoftLink::path() const
{
    auto const nr_bytes = value_size();

    std::string name;
    name.resize(nr_bytes, 'x');

#ifndef NDEBUG
    auto result =
#endif

    ::H5Lget_val(
        location_id(), this->name().c_str(),
// This test seems correct (201402L corresponds with C++14, non-const data()
// is introduced in C++17), but non-const data() does not seem to be
// available in GCC-5 and 6.
// #if __cplusplus > 201402L
//         name.data()
// #else
//         const_cast<std::string::value_type*>(name.data())
// #endif
        const_cast<std::string::value_type*>(name.data())
        , nr_bytes, H5P_DEFAULT);

#ifndef NDEBUG
    assert(result >= 0);
#endif

    name.resize(nr_bytes - 1);

    return name;
}


bool soft_link_exists(
    Group const& group,
    std::string const& name)
{
    return
        link_exists(group.id(), name) &&
        link_type(group.id(), name) == H5L_TYPE_SOFT;
}


SoftLink create_soft_link(
    Group& group,
    Identifier const& target,
    std::string const& name)
{
    auto status = ::H5Lcreate_soft(
        target.pathname().c_str(), group.id(), name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT);

    if(status < 0) {
        throw std::runtime_error(fmt::format(
            "Cannot create soft link {} at {}",
            name, target.pathname()
        ));
    }

    return SoftLink{group, name};
}

} // namespace hdf5
} // namespace lue
